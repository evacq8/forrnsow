// ~ synthesis.cpp ~
// This file is where the central audio loop & processing takes place.

#include "synthesis.hpp"
#include <cstdlib>
#include <stdio.h>
#include <math.h>

// Note object constructor
Note::Note() {
	active = false;
}

Notes::Notes() {
	// Create MAX_POLYPHONY amt of note slots and set them to inactive
	elements.resize(MAX_POLYPHONY);
	for (Note& n : elements) {
		n.active = false;
	}
}

// Handle deactivation and reuse from the Notes array
void Notes::handle_note_event(SynthMidiNoteEvent& note_event, uint32_t& current_frame) {
	if(current_frame < note_event.frame_offset) return; // Reject acting on note_events whose frame hasn't come yet
	// NOTE ON 
	if(note_event.note_on) {
		// Replace the first inactive note with this one.
		for(size_t i = 0; i < elements.size(); i++) {
			if(elements[i].active == true) continue;
			elements[i].active = true;
			elements[i].note_number = note_event.note;
			elements[i].phase = (double)rand() / RAND_MAX; // Randomize initial phase
			elements[i].increment = (440.0f * powf(2.0f, (note_event.note - 69.0f) / 12.0f)/44100); // ASSUMES SAMPLE RATE OF 44100
			return;
		}
	} 
	// NOTE OFF
	else {
		// Deactivate the first active note with this same note_number
		for(size_t i = 0; i < elements.size(); i++) {
			// Skip notes who are either inactive or have different note number than the event
			if(elements[i].active == false || elements[i].note_number != note_event.note) continue;
			elements[i].active = false;
			return;
		}
	}
}


// Synth object constructor
Synth::Synth() {

}

// Create notes object
Notes notes;

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
			if(notes[n].active) {
				mixed_sample += (float)sin(2.0 * M_PI * notes[n].phase);
				// Update phase for the next frame
				notes[n].phase += notes[n].increment;
				if(notes[n].phase >= 1.0) notes[n].phase -= 1.0;
			}
		}
		output_buffers[0][i] = mixed_sample * 0.2f;
		output_buffers[1][i] = mixed_sample * 0.2f;
	}
}

