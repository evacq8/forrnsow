#pragma once

// A class for tracking a note's envelopes
class Envelope {
	public:
		float level; // output gain of the envelope (0.0 - 1.0)

		// Settings: (time in seconds)
		float attack_time;
		float decay_time;
		float sustain_level;
		float release_time;
		// Tensions (controls curve shape)
		float attack_tension;
		float decay_tension;
		float release_tension;

		enum State {
			IDLE, // Note is not in use
			ATTACK, // Note is ramping up after noteOn
			DECAY, // Note is fading to sustain level
			SUSTAIN, // Note is at sustain level
			RELEASE // Note is fading away
		};
		State state;

		Envelope();
		// Update 'level' (also returns it)
		float update();
};
