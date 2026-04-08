#pragma once
#include <clap/clap.h>
#include "synthesis.hpp"
#include "fourier_transform.hpp"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <atomic>
#include <cmath>

class GUI {
	public: 
		GUI(const clap_window_t* window, struct Synth* synth);
		~GUI();
		void render();

		// ImGui
		ImGuiContext* imgui_context;
		// X11 / GLX Variables
		Display* display = nullptr;
		Window parent_window = 0; // DAW window
		Window child_window = 0; // Plugin window
		GLXContext glContext = nullptr;
		XVisualInfo* visualInfo = nullptr;

		struct Synth* synth;
};
