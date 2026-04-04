#pragma once
#include <math.h>


// A pointer to a oscillator function which
// ..takes in phase and outputs value from -1.0 - 1.0
typedef float (*OscillatorFunction)(float);
// Oscillator functions
namespace Waveforms {
	inline float sine(float phase) {
		return sin(phase * 2 * M_PI);
	};
	inline float sawtooth(float phase) {
		return (phase * 2.0f) - 1.0f;
	};
}

class Oscillator {
	public:
		float phase; // Current phase of the oscillator (0.0 - 1.0)
		float increment; // How much to increment oscillator per frame?
		OscillatorFunction function;

		Oscillator();
		// Updates increment to match a target frequency (Hz)
		// Sample rate is required for the calculation
		void set_frequency(float target_freq, int sample_rate);
		// Updates increment to match a target midi note frequency
		// Sample rate is required for the calculation
		void set_frequency_from_midi_note(int note, int sample_rate);
		// Update phase based on increment and return function(phase)
		// oscillator functions will return range between -1.0 - 1.0
		float tick();
};
