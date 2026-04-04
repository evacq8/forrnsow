CXX = g++
CXXFLAGS = -O3 -fPIC -std=c++17 -Wall
INCLUDES = 	-Iinclude \
			-Iextern/clap/include \
			-Iextern/clap-helpers/include
LDFLAGS = -shared

TARGET = forrnsow.clap
SRC = src/entry.cpp \
	  src/synthesis.cpp \
	  src/envelope.cpp \
	  src/oscillator.cpp \
	  src/biquad.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

install:
	mkdir -p ~/.clap
	cp $(TARGET) ~/.clap/
