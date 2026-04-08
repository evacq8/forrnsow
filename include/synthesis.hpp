#pragma once
#include <stdint.h>
#include <vector>
#include <atomic>

#include "envelope.hpp"
#include "oscillator.hpp"
#include "biquad.hpp"

#define MAX_POLYPHONY 20 // Notes array size
#define GUI_AUDIO_BUFFER_SIZE 1024 // Buffer size that is fed into gui

// Struct used for sending Midi note event info to the synth
struct SynthMidiNoteEvent {
	uint32_t frame_offset; // Which frame # did the event happen in the current buffer?
	int16_t note; // MIDI note number where 0 is a C-1 and 127 is a G9
	double velocity; // How hard note was pressed or released, normalized to 0.0 - 1.0
	bool note_on; // is this a Note on event? False means it's a noteOff event.
};

// A single note
class Note {
	public:
		uint16_t note_number; // MIDI note number where 0 is a C-1 and 127 is a G9
		Envelope envelope;
		Oscillator primary_oscillator;
		BiquadBandpassBank formants;

		Note();
};
// A class to manage the entire collective of notes
class Notes {
	private:
		std::vector<Note> elements;
	public:
		Notes();

		// Call every frame
		void handle_note_event(SynthMidiNoteEvent& note_event, uint32_t& current_frame);

		// Make calling Notes[i] give the vector index
		Note& operator[](size_t index) { return elements[index]; }
		size_t size() const { return elements.size(); }
};

class Synth {
	public:
		Notes notes;
		Synth();
		void process(float** output_buffers, uint32_t buffer_size, std::vector<SynthMidiNoteEvent>& midi_note_events);
		// This function is used to copy the gui audio ring buffer from audio thread to gui thread.
		void copy_gui_audio_buffer(float* destination);
	private:
		// A ring buffer for gui to grab audio with thread safety
		float gui_audio_buffer[GUI_AUDIO_BUFFER_SIZE] = {0};
		// An index to keep track of current write position in the ring buffer
		// It is atomic because it is modified in audio thread and read in gui thread
		std::atomic<int> gui_audio_buffer_write_idx{0};
		// Helper function to update the gui ring buffer every audio frame
		void update_gui_audio_buffer(float& sample);
};
