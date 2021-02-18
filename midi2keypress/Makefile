CXXFLAGS = -I/usr/include/libevdev-1.0
LDFLAGS = -levdev
OBJS = midi2keypress.o input.o

all: midi2keypress
midi2keypress: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

