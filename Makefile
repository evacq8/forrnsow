CXX = g++
CXXFLAGS = -O3 -fPIC -std=c++17 -Wall
INCLUDES = 	-Iinclude \
			-Iextern/clap/include \
			-Iextern/clap-helpers/include \
			-Iextern/imgui \
			-Iextern/imgui/backends
LDFLAGS = -shared -lX11 -lGL

TARGET = forrnsow.clap
SRC = src/entry.cpp \
	  src/synthesis.cpp \
	  src/envelope.cpp \
	  src/oscillator.cpp \
	  src/biquad.cpp \
	  src/gui.cpp \
	  src/fourier_transform.cpp

IMGUI_SRC = extern/imgui/imgui.cpp \
			extern/imgui/imgui_draw.cpp \
			extern/imgui/imgui_widgets.cpp \
			extern/imgui/imgui_tables.cpp \
			extern/imgui/imgui_demo.cpp \
			extern/imgui/backends/imgui_impl_opengl3.cpp

SRC += $(IMGUI_SRC)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install:
	mkdir -p ~/.clap
	cp $(TARGET) ~/.clap/
