#include "fourier_transform.hpp"

#include <complex>
#include <cmath>

void discrete_fourier_transform(const float* input, float* output, int input_size) {
	// Number of (possible) output freqeuncies
	int num_bins = input_size/2;
	// Loop through every frequency
	for(int k = 0; k < num_bins; k++) {
		// Center of mass sum
		std::complex<double> sum(0, 0);
		// Now coil the samples around in a circle
		for(int i = 0; i < input_size; i++) {
			// Calculate this samples position in the circle
			double angle = -2.0 * M_PI * k * i / input_size;
			// Turn it into a complex number (euler's formula)
			std::complex<double> rotation(cos(angle), sin(angle));
			// Multiplying this unit-circle rotation by input float (-1.0 - 1.0)
			// .. Draws that sample on the circle given its amplitude.
			// Add this tto the center of mass.
			sum+=(double)input[i]*rotation;
		}
		// Take the average of the mass sum to get average center of mass
		// Which says how prominent frequency k is in the signal.
		output[k] = std::abs(sum) / input_size;
	}
}

void apply_hann_window(float* buffer, int n) {
	for (int i = 0; i < n; i++) {
		float multiplier = 0.5*(1.0-cos(2.0*M_PI*i/(n-1)));
		buffer[i] *= multiplier;
	}
}
