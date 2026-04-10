// ~ synthesis.cpp ~
// This file is where the central audio loop & processing takes place.

#include "synthesis.hpp"
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <atomic>

// Note object constructor
Note::Note() {
	envelope.state = Envelope::IDLE;
	primary_oscillator.function = Waveforms::sawtooth;
	vibrato_oscillator.function = Waveforms::sine;
	vibrato_oscillator.set_frequency(5.5f, 44100);
	// formants for the 'ahh' vowel
	formants.add(650.0f,  1.2f, 1.0f, 44100);
    formants.add(1080.0f, 2.5f, 0.4f, 44100);
    formants.add(2650.0f, 4.0f, 0.2f, 44100);
    formants.add(3200.0f, 6.0f, 0.3f, 44100);
}

Notes::Notes() {
	// Create MAX_POLYPHONY amt of note slots and set them to inactive
	elements.resize(MAX_POLYPHONY);
	for (Note& n : elements) {
		n.envelope.state = Envelope::IDLE;
	}
}

// Handle deactivation and reuse from the Notes array
void Notes::handle_note_event(SynthMidiNoteEvent& note_event, uint32_t& current_frame) {
	if(current_frame < note_event.frame_offset) return; // Reject acting on note_events whose frame hasn't come yet
	// NOTE ON 
	if(note_event.note_on) {
		// Replace the first inactive note with this one.
		for(size_t i = 0; i < elements.size(); i++) {
			if(elements[i].envelope.state != Envelope::IDLE && elements[i].envelope.state != Envelope::RELEASE) continue;
			elements[i].envelope.state = Envelope::ATTACK;
			elements[i].note_number = note_event.note;
			elements[i].formants.reset_history();
			elements[i].primary_oscillator.set_frequency_from_midi_note(
				note_event.note,
				44100
			); // ASSUMES SAMPLE RATE OF 44100
			return;
		}
	} 
	// NOTE OFF
	else {
		// Deactivate the first active note with this same note_number
		for(size_t i = 0; i < elements.size(); i++) {
			// Skip notes who are either inactive or have different note number than the event
			if(elements[i].envelope.state == Envelope::IDLE || elements[i].note_number != note_event.note) continue;
			elements[i].envelope.state = Envelope::RELEASE;
			return;
		}
	}
}


// Synth object constructor
Synth::Synth() {

}

// Add a sample to the gui audio ring buffer
void Synth::update_gui_audio_buffer(float& sample) {
	// Get current write index (to write at that point)
	int idx = gui_audio_buffer_write_idx.load(std::memory_order_relaxed);
	gui_audio_buffer[idx] = sample;

	// Increment the write index and wrap if neccessary
	gui_audio_buffer_write_idx.store((idx+1) % GUI_AUDIO_BUFFER_SIZE, std::memory_order_relaxed);
}

// Called from gui thread!
void Synth::copy_gui_audio_buffer(float* destination) {
	// Get current write index
	int idx = gui_audio_buffer_write_idx.load(std::memory_order_relaxed);
	for(int i = 0; i < GUI_AUDIO_BUFFER_SIZE; i++) {
		destination[i] = gui_audio_buffer[(idx+i)%GUI_AUDIO_BUFFER_SIZE];
	}
}

// ~ MAIN PROCESS LOOP ~
// Output must be between -1.0 - 1.0
void Synth::process(float** output_buffers, uint32_t buffer_size, std::vector<SynthMidiNoteEvent>& midi_note_events) {
	size_t midi_event_ptr = 0;
	// Loop through buffer
	for(uint32_t i = 0; i < buffer_size; ++i) {
		// ~ Activate/deactivate notes based on midi note events ~
		while(midi_event_ptr < midi_note_events.size() && 
              midi_note_events[midi_event_ptr].frame_offset == i) {
            notes.handle_note_event(midi_note_events[midi_event_ptr], i);
            midi_event_ptr++; 
        }

		// Audio
		float mixed_sample = 0.0f;
		for(size_t n = 0; n < notes.size(); n++) {
			if(notes[n].envelope.state == Envelope::IDLE) continue;

			// Update envelope levelz
			float gain = notes[n].envelope.update();

			// ~ Modulate frequency ~
			// Calculate a new increment (i) from old increment (i_0) but plus N semitones:
			// i = i_0 * 2^(N/12)
			// N = oscillator_output * semitones
			float modulated_increment = (
						notes[n].primary_oscillator.increment *
						powf(2.0f, (notes[n].vibrato_oscillator.tick()*0.3f)/12.0f)
					);
			// Calculate the difference between the two so we know how much to add to reach the new modulated frequency.
			float increment_diff = 	notes[n].primary_oscillator.increment - 
									modulated_increment;
			// Add it.
			notes[n].primary_oscillator.phase+=increment_diff;

			// Generate Sample
			float sample = notes[n].primary_oscillator.tick() * gain * 0.2f;
			//sample = notes[n].formants.process(sample);
			mixed_sample += sample;
		}
		output_buffers[0][i] = mixed_sample;
		output_buffers[1][i] = mixed_sample;
		// Update gui audio buffer with this sample
		update_gui_audio_buffer(mixed_sample);
	}
}

