#include "biquad.hpp"


// Direct Form I
float Biquad::process(float x0) {
	float y0 = (b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2);

	x2 = x1;
	x1 = x0;

	y2 = y1;
	y1 = y0;

	return y0;
}


void Biquad::set_band_pass(float freq, float bandwidth, int sample_rate) {
	float omega = 2*M_PI*(freq/sample_rate);
	float alpha = sinf(omega)/(2*bandwidth);

	b0 = alpha;
	b1 = 0;
	b2 = -alpha;
	float a0 = 1 + alpha;
	a1 = -2*cosf(omega);
	a2 = 1 - alpha;

	b0/=a0;
	b1/=a0;
	b2/=a0;
	a1/=a0;
	a2/=a0;
}

void Biquad::reset_history() {x1 = x2 = y1 = y2 = 0;}


void BiquadBandpassBank::add(float freq, float bandwidth, float gain, int sample_rate) {
	Biquad biquad;
	biquad.set_band_pass(freq, bandwidth, sample_rate);

	// Pair biquad with its gain amt
	BiquadBandpassElement element;
	element.biquad = biquad;
	element.gain = gain;

	elements.push_back(element);
}

float BiquadBandpassBank::process(float sample) {
	float final_sample = sample*0; // TEMP WET MODE! 
	for (auto& element : elements) {
		float biquad_output = element.biquad.process(sample);
		final_sample += element.gain * biquad_output;
	}
	return final_sample;
}

void BiquadBandpassBank::reset_history() {
	for (auto& element : elements) {
		element.biquad.reset_history();
	}
}
