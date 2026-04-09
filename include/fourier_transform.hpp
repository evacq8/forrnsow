#pragma once

#include <complex>

// An implementation of the slow fourier transform O(N^2)
// Please pass a signal snippet where the borders are faded out (hann window)
// To avoid messing with the center of mass
void discrete_fourier_transform(const float* input, float* output, int input_size);

// Cooley-Tukey (Radix-2)
void fast_fourier_transform(const float* input, float* output, int N);

void apply_hann_window(float* buffer, int n);
