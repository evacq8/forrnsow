// ~ synthesis.cpp ~
// This file is where the central audio loop & processing takes place.

#include "synthesis.hpp"
#include <cstdlib>
#include <stdio.h>


// Synth object constructor
Synth::Synth() {

}

// ~ MAIN PROCESS LOOP ~
// Output must be between -1.0 - 1.0
void Synth::process(float** output_buffers, uint32_t buffer_size, std::vector<SynthMidiNoteEvent>& midi_note_events) {
	// ~ Get Midi ~
	for(size_t i = 0; i < midi_note_events.size(); i++) {
		printf("Midi note %d frame %d velocity %f Is on? %s\n", 
				midi_note_events[i].note, 
				midi_note_events[i].frame_offset, 
				midi_note_events[i].velocity,
				midi_note_events[i].note_on ? "Yes" : "No"
		);
		fflush(stdout);
	}
	
	// ~ Sound ~
	for(uint32_t i = 0; i < buffer_size; ++i) {
		output_buffers[0][i] = (float)rand() / RAND_MAX; // Left channel
		output_buffers[1][i] = (float)rand() / RAND_MAX; // Right channel
	}
}

