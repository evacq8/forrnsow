#include "gui.hpp"

GUI::GUI(const clap_window_t* window, struct Synth* the_synth) {
	synth = the_synth;

	display = XOpenDisplay(NULL);
	parent_window = (Window)window->x11;
	int attributes[] = {
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER,
		None
	};
	visualInfo = glXChooseVisual(display, DefaultScreen(display), attributes);
	// Create child "plugin" window.
	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(display, parent_window, visualInfo->visual, AllocNone);
	swa.event_mask = 	ExposureMask | 
						StructureNotifyMask | 
						ButtonPressMask | 
						ButtonReleaseMask |
						PointerMotionMask |
						KeyPressMask |
						KeyReleaseMask;
	child_window = XCreateWindow(display, parent_window,
			0, 0, 800, 600, 0,
			visualInfo->depth, InputOutput,
			visualInfo->visual,
			CWColormap | CWEventMask, &swa);

	XMapWindow(display, child_window);
	XFlush(display);

	glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, child_window, glContext);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// ImGui
	IMGUI_CHECKVERSION();
	imgui_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(imgui_context);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 130");
};

GUI::~GUI() {
	// ImGui
	ImGui::SetCurrentContext(imgui_context);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	// X11
	glXMakeCurrent(display, None, NULL);
	if (glContext) glXDestroyContext(display, glContext);
	if(child_window) XDestroyWindow(display, child_window);
	XCloseDisplay(display);
	display = nullptr;
}

void GUI::render() {
	ImGui::SetCurrentContext(imgui_context);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(800, 600);
	io.DeltaTime = 1.0f / 60.0f;

	XEvent ev;

	// Handle mouse events
	while (XPending(display)) {
		XNextEvent(display, &ev);
		if(ev.type == MotionNotify) {
			io.AddMousePosEvent((float)ev.xmotion.x, (float)ev.xmotion.y);
		} else if (ev.type == ButtonPress || ev.type == ButtonRelease) {
			bool down = (ev.type == ButtonPress);
			if(ev.xbutton.button == Button1) io.AddMouseButtonEvent(0, down); // Left click
			if(ev.xbutton.button == Button3) io.AddMouseButtonEvent(1, down); // Right click
		}
	}
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGuiWindowFlags window_flags =	ImGuiWindowFlags_NoTitleBar |
									ImGuiWindowFlags_NoResize |
									ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoCollapse;
	// Draw!
	ImGui::Begin("FORRNSOW", nullptr, window_flags);

	ImGui::Text("[forrnsow]");

	float audio[GUI_AUDIO_BUFFER_SIZE];
	float spectrum[GUI_AUDIO_BUFFER_SIZE/2];
	synth->copy_gui_audio_buffer(audio);

	ImGui::PlotLines("Oscilloscope", audio, GUI_AUDIO_BUFFER_SIZE, 0, NULL, -1.0f, 1.0f, ImVec2(0,150));

	apply_hann_window(audio, GUI_AUDIO_BUFFER_SIZE);
	discrete_fourier_transform(audio, spectrum, GUI_AUDIO_BUFFER_SIZE);

	// DRAW SPECTRAL MAP
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	float width = ImGui::GetContentRegionAvail().x;
	float height = 150.0f;
	const float min_freq = 20.0f;
	const float max_freq = 20000.0f;
	draw_list->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(30, 30, 30, 255)); // Draw background rectangle
	// Draw bin bars
	for (int x = 0; x < (int)width; x++) {
		float t = (float)x/width;
		float freq = min_freq * pow(max_freq / min_freq, t);
		int bin = (int)(freq / (44100.0f / GUI_AUDIO_BUFFER_SIZE));
		if(bin>=0 && bin < GUI_AUDIO_BUFFER_SIZE / 2) {
			float magnitude = spectrum[bin];
			float h = magnitude * 3000.0f;
			if (h > height) h = height;
			draw_list->AddLine(
				ImVec2(pos.x + x, pos.y + height),
				ImVec2(pos.x + x, pos.y + height - h),
				IM_COL32(255, 255, 255, 255)
			);
		}
	}
	ImGui::Dummy(ImVec2(0, height));

	ImGui::End();
	// Draw end...
	
	ImGui::Render();
	glXMakeCurrent(display, child_window, glContext);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//Swap buffers
	glXSwapBuffers(display, child_window);
}





