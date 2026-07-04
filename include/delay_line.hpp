#include <vector>

class DelayLine {
	public:
		// Allocates memory for the DelayLine
		DelayLine(float max_delay, int sample_rate);
		// Returns the output sample from the DelayLine
		// Takes in the input sample to add to the delay buffer.
		float tick(float input);
		// How long should the delayline wait until feeding back a sample?
		// Must be less than max_delay (entered on construction)
		void set_delay(float seconds, int sample_rate);
	private: 
		// Ring buffer to store audio to play back
		float buffer_size;
		std::vector<float> buffer;
		// Write/read "pin"/indicies to write and read data to the circular buffer
		float read_pin;
		int write_pin;
		float delay_in_samples;
};
