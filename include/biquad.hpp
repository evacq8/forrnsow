#pragma once
#include <math.h>
#include <vector>

// An instance of a biquad filter
class Biquad {
	private:
		// Coefficients
		float b0, b1, b2, a1, a2;
		// Previous two inputs
		float x1, x2 = 0.0f;
		// Previous two outputs
		float y1, y2 = 0.0f;

	public:
		// x0 is the current input
		float process(float x0);
		void set_band_pass(float freq, float bandwidth, int sample_rate);
		// Zero out history
		void reset_history();
};

// A object to handle a collection of bandpasses which process a sample in parallel and add or subtract their output from the sample
class BiquadBandpassBank {
	private:
		// Used to manage biquad instances with their respective gains
		struct BiquadBandpassElement {
			Biquad biquad;
			float gain;
		};
		std::vector<BiquadBandpassElement> elements;
	public:
		void add(float freq, float bandwidth, float gain, int sample_rate); // Add a bandpass
		float process(float sample); // Process all biquads in parallel and add them to the input sample based on gain
		void reset_history(); // Clear history of all biquads
};
