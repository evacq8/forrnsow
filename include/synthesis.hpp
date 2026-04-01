#pragma once
#include <stdint.h>
#include <vector>

// Struct used for sending Midi note event info to the synth
struct SynthMidiNoteEvent {
	uint32_t frame_offset; // Which frame # did the event happen in the current buffer?
	int16_t note; // MIDI note number where 0 is a C-1 and 127 is a G9
	double velocity; // How hard note was pressed or released, normalized to 0.0 - 1.0
	bool note_on; // Note on? False means it's a noteOff event.
};

class Synth {
	public:
		Synth();
		void process(float** output_buffers, uint32_t buffer_size, std::vector<SynthMidiNoteEvent>& midi_note_events);
};
