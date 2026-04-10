#include "gui.hpp"
#include "fonts/SourceCodeProFonts.h"

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

	ImGuiIO& io = ImGui::GetIO();
	fontRegular = io.Fonts->AddFontFromMemoryCompressedTTF(SourceCodePro_Regular_compressed_data, SourceCodePro_Regular_compressed_size, 16.0f);
	fontMedium = io.Fonts->AddFontFromMemoryCompressedTTF(SourceCodePro_Medium_compressed_data, SourceCodePro_Medium_compressed_size, 16.0f);
	fontBold = io.Fonts->AddFontFromMemoryCompressedTTF(SourceCodePro_Bold_compressed_data, SourceCodePro_Bold_compressed_size, 16.0f);
	fontRegularHeader = io.Fonts->AddFontFromMemoryCompressedTTF(SourceCodePro_Regular_compressed_data, SourceCodePro_Regular_compressed_size, 24.0f);

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

void GUI::draw() {
	ImGui::Begin("FORRNSOW", nullptr, window_flags);

	// DRAW TOP HEADER
	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Padding
	// Button style
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.19f, 0.19f, 0.19f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 1.0f));
	// Text Colour
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
	// Indent
	ImGui::Indent(14.0f);

		ImGui::PushFont(fontRegularHeader);
		ImGui::Text("[forrnsow]");
		ImGui::SameLine(0.0f, 20.0f);
		if(ImGui::Button("home")) current_tab = TAB_HOME;
		ImGui::SameLine(0.0f, 20.0f);
		if(ImGui::Button("visuals")) current_tab = TAB_VISUALS;
		ImGui::PopFont();

	// Pop stuff
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(4);
	ImGui::Unindent(14.0f);
	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Padding

	// END DRAW HEADER
	// DRAW MAIN WINDOW

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); 
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

	ImGui::Indent(10.0f); // Padding-left
	float child_width = ImGui::GetContentRegionAvail().x - 10.0f;
	float child_height = ImGui::GetContentRegionAvail().y - 10.0f;
    if (ImGui::BeginChild("MainContent", ImVec2(child_width, child_height), false)) 
    {
		if(current_tab == TAB_HOME) {
				// Text Colour
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
				ImGui::Text("Hello, world!");
				ImGui::PopStyleColor();
		} else if(current_tab == TAB_VISUALS) {
			// How much space to leave at the bottom of the chart (for text/lables)
			float bottom_size = 30.0f;
			// Where to show freq text
			float freq_labels[] = {50, 100, 200, 300, 500, 2000, 3000, 5000, 10000, 18000};
			// Draw spectrum chart
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
			ImVec2 space_avail = ImGui::GetContentRegionAvail();
			space_avail.y -= bottom_size;
			float min_hz = 20.0f;
			float max_hz = 22000.0f;

			float audio[GUI_AUDIO_BUFFER_SIZE];
			synth->copy_gui_audio_buffer(audio);
			float freq_bins[GUI_AUDIO_BUFFER_SIZE/2];
			fast_fourier_transform(audio, freq_bins, GUI_AUDIO_BUFFER_SIZE);
			
			float prev_draw_x = 0.0f;
			float prev_draw_y = 0.0f;
			// Loop through every pixel
			for(int x = 0; x < (int)space_avail.x; x++) {
				float percentage = (float)x/space_avail.x;
				// Get the freq this pixel represents in the logarithmic scale
				float current_hz = min_hz * powf(max_hz / min_hz, percentage);
				// Convert the freq to FFT bin index
				float bin_idx_float = current_hz*(GUI_AUDIO_BUFFER_SIZE/44100.0f);
				// Get the upper and lower values (linear interpolate)
				float bin_upper = freq_bins[(x<space_avail.x-1) ? (int)bin_idx_float+1 : int(bin_idx_float)];
				float bin_lower = freq_bins[(int)bin_idx_float];
				// Final magnitude
				float interpolated = (bin_upper-bin_lower) * (bin_idx_float - (float)((int)bin_idx_float)) + bin_lower;
				float db = 20.0f * log10f(interpolated+1e-6f);
				// convert decibels to a y value from 0.0 - 1.0
				float normalized_y = (db + 60.0f) / 60.0f;
				if(normalized_y > 1.0f) normalized_y = 1.0f;
				if(normalized_y < 0.0f) normalized_y = 0.0f;
				// convert normalized y to drawing coordinates of this pixel
				float draw_y = canvas_pos.y + space_avail.y - (normalized_y * space_avail.y);
				float draw_x = canvas_pos.x + x;
				if (x > 0) {
					draw_list->AddLine(
						ImVec2(prev_draw_x, prev_draw_y), 
						ImVec2(draw_x, draw_y), 
						IM_COL32(180, 180, 180, 255), 
						2.0f // Thickness
					);
				}
				prev_draw_x = draw_x;
				prev_draw_y = draw_y;
			}
			// Draw frequency labels
			for(float hz : freq_labels) {
				float t = logf(hz / min_hz) / logf(max_hz / min_hz);
				float x_pos = canvas_pos.x + (t * space_avail.x);
				char buf[32];
				// add kilo prefix when above 1000
				if (hz >= 1000) sprintf(buf, "%dk", (int)hz/1000); else sprintf(buf, "%d", (int)hz);
				draw_list->AddText(ImVec2(x_pos, canvas_pos.y + space_avail.y + 2), IM_COL32(150, 150, 150, 255), buf);
			}
			ImGui::Dummy(ImVec2(0.0f, bottom_size+space_avail.y));
		}
    }
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
    ImGui::EndChild();
	ImGui::Unindent(10.0f);

	// END DRAW MAIN WINDOW

	ImGui::End();
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

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

	// Actually draw stuff to the frame
	GUI::draw();
	
	ImGui::Render();
	glXMakeCurrent(display, child_window, glContext);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//Swap buffers
	glXSwapBuffers(display, child_window);
}





