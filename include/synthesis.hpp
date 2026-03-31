#pragma once
#include <stdint.h>

class Synth {
	public:
		Synth();
		void process(float** output_buffers, uint32_t buffer_size);
};
