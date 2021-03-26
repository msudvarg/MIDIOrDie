CXXFLAGS = -g `pkg-config --cflags rtmidi` -Ilocalcontroller
LDFLAGS = `pkg-config --libs rtmidi` -Llocalcontroller -llc -lportaudio -lfftw3 -Wl,-R,localcontroller
OBJS = midi2keypress.o midi.o tone.o

all: midi2keypress
midi2keypress: $(OBJS) localcontroller/liblc.so
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) 
localcontroller/liblc.so:
	$(MAKE) -C localcontroller liblc.so
clean:
	-rm *.so
	-rm *.o

# Hey Marion, if you're reading this because you got an error about liblc.so
# not linking, add it to your LD_LIBRARY_PATH. Eg:
# export LD_LIBRARY_PATH=LD_LIBRARY_PATH:/home/marion/pathto/MidiOrDie/midi2keypress/localcontroller