#include "fourier_transform.hpp"

#include <cmath>
#include <vector>

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


// Reorder data to mimic recursive even/odd splits
// Only works if N is a power of 2
void bit_reverse(std::vector<std::complex<float>>& data, int N) {
	// Loop through every index
	int j = 0;
	for (int i = 0; i < N; i++) {
		if(i < j) std::swap(data[i], data[j]); // prevent unswaping

		// This bit acts as a counter for bit manipulation
		// Start at the Most Significant Bit which is required to represent N
		int target_bit = N >> 1;
		// If there are 1's hogging the MSB and possibly the subsequent bits
		// ..delete them and move right (towards LSB) until we reach a 0, 
		while(j & target_bit) {
			j ^= target_bit; // flip the 1 to 0
			target_bit >>= 1; // move the counter to the right
		}
		// ..then replace that 0 with a 1.
		// If there isn't any 1's hogging the MSB just replace the MSB w/ 1!
		j ^= target_bit;
	}
}

// Cooley-Tukey (Radix-2)
// Works only when N is a power of 2
void fast_fourier_transform(const float* input, float* output, int N) {
	// Turn input into complex numbers with imaginary parts of 0
	std::vector<std::complex<float>> input_complex(N);
	for (int i = 0; i < N; i++)
		input_complex[i] = std::complex<float>(input[i], 0);
	// Bit-reverse indices
	bit_reverse(input_complex, N);

	// Loop through sub-group sizes (powers of 2) until we reach N
	for (int s = 2; s <= N; s*=2) {
		// Calculate twiddle step amount
		float twiddle_step_angle = -2.0f * M_PI / s;
		std::complex<float> twiddle_step(
				std::cos(twiddle_step_angle), 
				std::sin(twiddle_step_angle));

		// Loop through sub-groups
		for (int k = 0; k < N; k+=s) {
			// Twiddle factor (start at 0 deg rotation)
			std::complex<float> twiddle(1.0f, 0.0f);
			// Loop through the first half of this sub-group
			for (int j = 0; j < s / 2; j++) {
				std::complex<float> u = input_complex[k+j];
				std::complex<float> t = twiddle * input_complex[k+j+s/2];

				input_complex[k + j] = u+t;
				input_complex[k + j + s / 2] = u-t;
				twiddle*=twiddle_step;
			}
		}
	}

	// Store frequency bin magnitudes to output
	for (int k = 0; k < N/2; k++) {
		output[k] = std::abs(input_complex[k])/N;
	}
}

void apply_hann_window(float* buffer, int n) {
	for (int i = 0; i < n; i++) {
		float multiplier = 0.5*(1.0-cos(2.0*M_PI*i/(n-1)));
		buffer[i] *= multiplier;
	}
}
