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

NodeOscillator::NodeOscillator(const HostContext& context) : Node(context) {
	phase = 0; // TODO randomize this

	pins_in.push_back(InputPin({WireType::FREQUENCY}, "target freq"));
	pins_out.push_back(OutputPin({WireType::SIGNAL}, "osc output"));

	// Resize buffer to buffer size
	std::get<std::vector<double>>(pins_out[0].buffer).resize(host_context.buffer_size, 0.0);
}

void NodeOscillator::process() {
	if (pins_in[0].buffer == nullptr) std::cout << "osc node: No frequency pin connected\n";

	const auto& target_freq_buffer = std::get<std::vector<double>>(*pins_in[0].buffer);
	auto& output_buffer = std::get<std::vector<double>>(pins_out[0].buffer);

	for (int f = 0; f < host_context.buffer_size; f++) {
		phase += target_freq_buffer[f] / host_context.sample_rate;
		while (phase >= 1.0) phase -= 1.0;
		output_buffer[f] = sin(phase * M_PI * 2.0);
	}
}

// Node: Mono Output

NodeMonoOutput::NodeMonoOutput(const HostContext& context) : Node(context) {
	pins_in.push_back(InputPin({WireType::SIGNAL}));
	pins_in[0].name = "final audio";
}

void NodeMonoOutput::process() {
	const auto& input_buffer = std::get<std::vector<double>>(*pins_in[0].buffer);
	std::copy(input_buffer.begin(), input_buffer.end(), host_context.output_buffers[0]);
	std::copy(input_buffer.begin(), input_buffer.end(), host_context.output_buffers[1]);
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

Synth::Synth() {
	host_context = HostContext();

	test_frequency_buffer = std::vector<double>(host_context.buffer_size, 440.0);

	global_nodes.push_back(std::make_unique<NodeOscillator>(host_context));
	global_nodes.push_back(std::make_unique<NodeMonoOutput>(host_context));
	global_nodes[0]->pins_in[0].buffer = &test_frequency_buffer;
	global_nodes[1]->pins_in[0].buffer = &global_nodes[0]->pins_out[0].buffer;
}

void Synth::process(float** output_bufs, uint32_t buffer_size, std::vector<MidiNoteEvent> midi_notes) {
    host_context.output_buffers = output_bufs;
    host_context.buffer_size = buffer_size;

	// TODO handle resizing elsewhere. not here
	// resize test buffer
    auto& freq_vec = std::get<std::vector<double>>(test_frequency_buffer);
    if (freq_vec.size() != buffer_size) {
        freq_vec.assign(buffer_size, 440.0);
    }

	// resize output pin buffers
    for (auto &node : global_nodes) {
        for (auto &out_pin : node->pins_out) {
            if (std::holds_alternative<std::vector<double>>(out_pin.buffer)) {
                auto& buf = std::get<std::vector<double>>(out_pin.buffer);
                if (buf.size() != buffer_size) {
                    buf.resize(buffer_size, 0.0);
                }
            }
        }
        
        node->process();
    }
}















