// ~ synthesis.cpp ~
// This file is where the central audio loop & processing takes place.

#include "synthesis.hpp"
#include <cstdlib>

// Synth object constructor
Synth::Synth() {

}

// ~ MAIN PROCESS LOOP ~
// Output must be between -1.0 - 1.0
void Synth::process(float** output_buffers, uint32_t buffer_size) {
	for(uint32_t i = 0; i < buffer_size; ++i) {
		output_buffers[0][i] = (float)rand() / RAND_MAX; // Left channel
		output_buffers[1][i] = (float)rand() / RAND_MAX;// Right channel
	}
}

