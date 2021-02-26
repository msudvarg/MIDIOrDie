CXXFLAGS = -g -I/usr/include/libevdev-1.0 -I../controller -I../games/localcontroller
LDFLAGS = -levdev -L../controller -L../games/localcontroller -llc
OBJS = midi2keypress.o input.o tone.o

all: midi2keypress
midi2keypress: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

