#include <synth.hpp>
#include <iostream>
#include <cmath>

// --
// -- Parameter
// --

Parameter::Parameter(ParameterType t, std::variant<double, bool> default_value) {
	type = t;
	data = default_value;
}

// --
// -- Specific Nodes
// --

// Node: Oscillator

NodeOscillator::NodeOscillator() {
	phase = 0; // TODO randomize this

	pins_in.push_back(InputPin({WireType::FREQUENCY}, "target freq"));
	pins_out.push_back(OutputPin({WireType::SIGNAL}, "osc output"));

	// Resize buffer to buffer size
	std::get<std::vector<double>>(pins_out[0].buffer).resize(BUFFER_SIZE, 0.0);
}

void NodeOscillator::process() {
	if (pins_in[0].prev_buffer == nullptr) std::cout << "osc node: No frequency pin connected\n";

	const auto& target_freq_buffer = std::get<std::vector<double>>(*pins_in[0].prev_buffer);
	auto& output_buffer = std::get<std::vector<double>>(pins_out[0].buffer);

	for (int f = 0; f < BUFFER_SIZE; f++) {
		phase += target_freq_buffer[f] / SAMPLE_RATE;
		while (phase >= 1.0) phase -= 1.0;
		output_buffer[f] = sin(phase * M_PI * 2.0);
	}
}

// Node: Mono Output

NodeMonoOutput::NodeMonoOutput() {
	pins_in.push_back(InputPin({WireType::SIGNAL}));
	pins_in[0].name = "final audio";
}

void NodeMonoOutput::process() {
	// TODO
}

// Node: Stereo Output

// Node: Frequency Domain Display

// Node: Band-pass

// Node: Midi Frequency - convert midi note to its frequency, also outputs a trigger event

// Node: Midi Velocity - outputs a note's velocity

// Node: ADSR Envelope
// * Takes in trigger event, outputs envelopes output amplitude

// Node: Signal Scaler
// * Changes the volume of a signal, takes in amplitude and signal and outputs modified signal

// Node: 



















