CXXFLAGS := -fPIC -g -O2 -std=c++14 -Igodot-cpp/include -Igodot-cpp/include/core -Igodot-cpp/include/gen -Igodot-cpp/godot-headers
LDFLAGS := -Lgodot-cpp/bin -lgodot-cpp.linux.debug.64 

all: bin/libMidiCPP.so

bin/libMidiCPP.so: src/init.o
	$(CXX) -o $@ -shared $^ $(LDFLAGS)
