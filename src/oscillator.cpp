#include "oscillator.hpp"

// Oscillator class

// Constructor
Oscillator::Oscillator() {
	// Randomize initial phase
	phase = (float)rand() / RAND_MAX;
}

void Oscillator::set_frequency(float target_freq, int sample_rate) {
	increment = target_freq/sample_rate;
}

void Oscillator::set_frequency_from_midi_note(int note, int sample_rate) {
	float freq = 440.0f * powf(2.0f, (note - 69.0f)/12.0f);
	increment = freq/sample_rate;
}

float Oscillator::tick() {
	// Update phase
	phase += increment;
	// Wrap the phase when it goes above 1.0
	while(phase >= 1) phase -= 1;
	// Return function(phase)
	return function(phase);
}
