// ~ entry.cpp ~
// This file handles letting the DAW identify the plugin, creating the plugin, and getting/setting start-up settings/info. 

#include <string.h>
#include <clap/clap.h>
#include <stdio.h>

// ~ 'Glue' headers that remove boiler plate. ~

// Main clap stuff.
#include <clap/helpers/plugin.hh>
// Contains HostProxy class which helps communicate with the DAW.
#include <clap/helpers/host-proxy.hh>
#include <clap/helpers/plugin.hxx>

// ~ Other files. ~

#include "synthesis.hpp"

// State the plugin features
static const char* plugin_features[] = {
	CLAP_PLUGIN_FEATURE_INSTRUMENT,
	CLAP_PLUGIN_FEATURE_SYNTHESIZER,
	nullptr // Signify end of array with nullptr
};
// Struct used to help the DAW identify the plugin and give plugin "meta-data."
static const clap_plugin_descriptor_t plugin_descriptor = {
	.clap_version = CLAP_VERSION_INIT,
	.id = "com.evacq8.forrnsow",
	.name = "FORRNSOW",
	.vendor = "evacq8",
	.url = "https://github.com/evacq8/forrnsow",
	.manual_url = "https://github.com/evacq8/forrnsow",
	.support_url = "https://github.com/evacq8/forrnsow",
	.version = "1.0.0",
	.description = "Haii",
	.features = plugin_features // Pass in features we defined above
};

// ~ Create the plugin object. ~

class Plugin : public clap::helpers::Plugin<
    clap::helpers::MisbehaviourHandler::Ignore, 
    clap::helpers::CheckingLevel::Maximal> 
{
public:
    Plugin(const clap_host_t *host) 
        : clap::helpers::Plugin<
            clap::helpers::MisbehaviourHandler::Ignore, 
            clap::helpers::CheckingLevel::Maximal>
          (&plugin_descriptor, host) 
    {}

	Synth synth;
    clap_process_status process(const clap_process_t *process) noexcept override {
		float** output_buffers = process->audio_outputs[0].data32;
		uint32_t buffer_size = process->frames_count;

		// ~ Midi ~
		// Create SynthMidiNoteEvents Vector from clap process midi data
		std::vector<SynthMidiNoteEvent> midi_events;
		for(uint32_t i = 0; i < process->in_events->size(process->in_events); i++) {
			// Get the generic event header pointer
			const clap_event_header_t* event_header = process->in_events->get(process->in_events, i);
			if(event_header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
			if(event_header->type == CLAP_EVENT_MIDI) {
				// Once verified its a midi event, cast it to a midi event
				clap_event_midi_t* event_midi = (clap_event_midi_t*)event_header;
				midi_events.push_back({
					event_header->time,
					event_midi->data[1],
					(double)event_midi->data[2]/127.0,
					(event_midi->data[0] & 0xF0) == 0x90 ? true : false
				});
			} else printf("Unknown event header type.\n");
		}

		// ~ Allow Synth to process, then continue ~
		synth.process(output_buffers, buffer_size, midi_events);
        return CLAP_PROCESS_CONTINUE; 
    }

	
	// ~ Audio Ports ~
	// Do we use audio ports? Yes.
	bool implementsAudioPorts() const noexcept override { return true; }
	// How many? 1 output, 0 input (we're an instrument, not a filter)
	uint32_t audioPortsCount(bool isInput) const noexcept override {
		return isInput ? 0 : 1;
	}
	// Tell the DAW more about the 1 output.
	bool audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info_t *info) const noexcept override {
		// Return in-case the DAW accidently asks about.. 
		// ..the wrong audio port index or an input
		if (isInput || index > 0) return false; 
		// Change settings under info object
		info->id = 0; 
		strncpy(info->name, "Main Output", sizeof(info->name)); // Set name
		info->flags = CLAP_AUDIO_PORT_IS_MAIN; // This is the main output port
		info->channel_count = 2; // Stereo output
		info->port_type = CLAP_PORT_STEREO;
		// No input pair to share memory with so give invalid ID.
		info->in_place_pair = CLAP_INVALID_ID; 
		return true;
	}

	// ~ Say we support MIDI ~
	bool implementsNotePorts() const noexcept override { return true; }
	uint32_t notePortsCount(bool isInput) const noexcept override { return isInput ? 1 : 0; }
	bool notePortsInfo(uint32_t index, bool isInput, clap_note_port_info_t *info) const noexcept override {
		if (!isInput || index > 0) return false;
		info->id = 0;
		info->supported_dialects = CLAP_NOTE_DIALECT_MIDI;
		strncpy(info->name, "Note Input", sizeof(info->name));
		return true;
	}
}; // End of Plugin class

// ~ Factory. ~

 static const clap_plugin *factory_create_plugin(
    const struct clap_plugin_factory *factory,
    const clap_host_t *host, // Used to get info from the DAW (e.g. Current BPM)
    const char *plugin_id // The plugin ID the daw wants to turn on (clap can hold multiple plugins)
) {
    // Check if the plugin daw wants to turn on matches with our plugin id
    if (strcmp(plugin_id, plugin_descriptor.id) != 0) return nullptr;

    // Create a new instance of our Synth object
    auto *my_plug = new Plugin(host);
    return my_plug->clapPlugin();
} 

static const clap_plugin_factory_t pluginFactory = {
    .get_plugin_count = [](const clap_plugin_factory *factory) -> uint32_t { return 1; }, // only 1 plugin
    .get_plugin_descriptor = [](const clap_plugin_factory *factory, uint32_t index) -> const clap_plugin_descriptor_t* {
		if (index == 0) return &plugin_descriptor;
		return nullptr;
	}, // feed in plugin descriptor
    .create_plugin = factory_create_plugin
};

// ~ Entry. ~

extern "C" {
    const clap_plugin_entry_t clap_entry = {
        .clap_version = CLAP_VERSION_INIT,
        .init = [](const char *path) -> bool { 
            printf(">>> [FORRNSOW] Initializing...\n");
            fflush(stdout); 
            return true; 
        },
        .deinit = []() {},
        .get_factory = [](const char *factory_id) -> const void * {
            // Check if the DAW is asking for the Main Plugin Factory
            if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) {
                return &pluginFactory;
            }
            return nullptr;
        }
    };
}
