CXXFLAGS = -g -I/usr/include/libevdev-1.0 -Ilocalcontroller
LDFLAGS = -levdev -Llocalcontroller -llc -Wl,-R,localcontroller
OBJS = midi2keypress.o input.o tone.o

all: midi2keypress
midi2keypress: $(OBJS) localcontroller/liblc.so
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
localcontroller/liblc.so:
	$(MAKE) -C localcontroller liblc.so
