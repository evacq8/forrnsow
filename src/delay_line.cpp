#include "delay_line.hpp"

DelayLine::DelayLine(float max_delay, int sample_rate) {
	// Allocate memory and zero it
	buffer.assign((int)(max_delay*sample_rate), 0);
	write_pin = 0;
	// Set it to max_delay for now...
	set_delay(max_delay, sample_rate);
}

void DelayLine::set_delay(float seconds, int sample_rate) {
	delay_in_samples = seconds*sample_rate;
}

float DelayLine::tick(float input) {
	// Update write_pin position
	write_pin = (write_pin+1) % buffer.size();
	// Update the read_pin position
	read_pin = write_pin - delay_in_samples;
	if(read_pin < 0) read_pin = read_pin+=buffer.size();
	// Add input to the tape
	buffer[write_pin] = input;
	// Return the output where the read_pin is (w/ linear interpolation)
	float lower_sample = buffer[(int)read_pin];
	float upper_sample = buffer[((int)read_pin+1) % buffer.size()];
	float frac = read_pin - (int)read_pin;
	return ((upper_sample-lower_sample)*frac) + lower_sample;
}
