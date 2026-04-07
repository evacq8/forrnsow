#pragma once
#include <stdint.h>
#include <vector>

#include "envelope.hpp"
#include "oscillator.hpp"
#include "biquad.hpp"

#define MAX_POLYPHONY 20

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
};
