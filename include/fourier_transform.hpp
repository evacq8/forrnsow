#pragma once

// An implementation of the slow fourier transform O(N^2)
// Please pass a signal snippet where the borders are faded out (hann window)
// To avoid messing with the center of mass
void discrete_fourier_transform(const float* input, float* output, int input_size);

void apply_hann_window(float* buffer, int n);
