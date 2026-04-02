// ~ envelope.cpp ~
// this file handles handling envelope changes and outputing a gain to tell a notes volume based on its stage

#include <envelope.hpp>

// Constructor
Envelope::Envelope() {
	attack_time = 0.005f;
	decay_time = 0.1f;
	sustain_level = 0.7f;
	release_time = 0.05f;

	level = 0.0f;
	state = IDLE;
}

// Update envelope level and state changes (except Idle->Attack and Any->Release) those are controlled in NoteOn and NoteOff
float Envelope::update() {
	float step = 0.0f; // How much to change level this frame?
	switch (state) {
		case IDLE:
			// Do nothing (Idle -> Attack is handled by NoteOn)
			break;
		case ATTACK:
			step = 1.0f/(attack_time*44100); // ASSUMES 44100 HZ
			// Attack -> Decay transition
			if(level+step > 1.0f) { 
				state = DECAY; 
				level = 1.0f; 
				return level; 
			}
			break;
		case DECAY:
			step = (sustain_level-1.0)/(decay_time*44100);
			// Decay -> Sustain transition
			if(level+step < sustain_level) { 
				state = SUSTAIN; 
				level = sustain_level; 
				return level; 
			}
			break;
		case SUSTAIN:
			// Do nothing (Any -> Release is handled by NoteOff)
			break;
		case RELEASE:
			step = -1.0f/(release_time*44100);
			// Release -> Idle transition
			if(level+step < 0.0f) { 
				state = IDLE; 
				level = 0.0f; 
				return level; 
			}
			break;
	}
	level += step;
	return level;
}





