CXXFLAGS = -g `pkg-config --cflags rtmidi` -Ilocalcontroller
LDFLAGS = `pkg-config --libs rtmidi` -Llocalcontroller -llc -Wl,-R,localcontroller
OBJS = midi2keypress.o midi.o tone.o

all: midi2keypress
midi2keypress: $(OBJS) localcontroller/liblc.so
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
localcontroller/liblc.so:
	$(MAKE) -C localcontroller liblc.so
