#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <string>
#include <variant>

struct MidiNoteEvent {
	uint32_t frame_offset; // which frame of the buffer did the event happen?
	int16_t note; // midi note number, 0 = C-1, 127 = G9
	double velocity; // how hard was the note pressed? normalized to 0.0-1.0
	bool note_on;
};

// Struct assigned to a synth instance that stores info such as sample_rate, output buffers, etc. etc.
// The daw will override most of these anyway, maybe except max_polyphony.
struct HostContext {
	double sample_rate = 44100;
	uint32_t buffer_size = 512;
	float** output_buffers = nullptr;
	int max_polyphony = 16;
};

// --
// -- PINS
// --

enum class WireType {
	SIGNAL, // Handles buffers of audio (0.0-1.0 double)
	FREQUENCY, // Handles buffer of frequency in Hz (double)
	TRIGGER, // A single impulse to indicate an event like a note press, a single true boolean in a stream of false. (boolean)
};
using PinBuffer = std::variant<std::vector<double>, std::vector<bool>>;
struct InputPin {
	std::vector<WireType> supported_wires;
	std::string name;
	// Holds pointer to the buffer of the output pin connected to this one, if null that means no connection.
	PinBuffer* buffer = nullptr;
	InputPin(std::vector<WireType> supported_wires, std::string name = "unnamed pin") : supported_wires(supported_wires), name(name) {};
};
struct OutputPin {
	std::vector<WireType> supported_wires;
	std::string name;
	// Pin's owner writes final output to this buffer
	PinBuffer buffer;
	OutputPin(std::vector<WireType> supported_wires, std::string name = "unnamed pin") : supported_wires(supported_wires), name(name) {};
};

// --
// -- PARAMETER
// -- * a setting attached to a node that controls an aspect of it

enum class ParameterType {
	FREQUENCY, // Parameter in Hz (double)
	DURATION, // Parameter in seconds (double)
	BOOLEAN, // Yes or no parameter (bool)
	FLOAT, // Generic floating point number (double)
};
class Parameter {
	std::variant<double, bool> data;
	ParameterType type;
	Parameter(ParameterType type, std::variant<double, bool> default_value);
};

// --
// -- NODE
// --

class Node {
protected:
	const HostContext& host_context; // a pointer to the synth's HostContext so this node knows required info (e.g. sample rate)
public:
	std::vector<InputPin> pins_in;
	std::vector<OutputPin> pins_out;
	std::vector<Parameter> params;

	Node(const HostContext& context) : host_context(context) {}
	virtual void process() = 0;
	virtual ~Node() = default;
};

// --
// -- SPECIFIC NODE TYPES
// --

class NodeOscillator : public Node {
private:
	double phase = 0.0; // 0.0-1.0 (not 0.0 to 2π)
public:
	void process();
	NodeOscillator(const HostContext& context);
};

class NodeMonoOutput : public Node {
public:
	void process();
	NodeMonoOutput(const HostContext& context);
};

// --
// -- SYNTH TODO
// -- * an instance of the synth
class Synth {
public:
	HostContext host_context;
	std::vector<std::unique_ptr<Node>> global_nodes;
	// TESTING
	PinBuffer test_frequency_buffer;
	Synth();
	void process(float** output_bufs, uint32_t buffer_size, std::vector<MidiNoteEvent> midi_notes);
};
