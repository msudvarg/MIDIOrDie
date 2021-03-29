CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp rtmidi`
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp rtmidi`
SOCKET = ../socket/libsocket.a
FFT2MIDIDIR = ../fft2midi
FFT2MIDI = $(FFT2MIDIDIR)/fft2midi.o
FFT2MIDIOBJS = $(FFT2MIDIDIR)/midi.o $(FFT2MIDIDIR)/tone.o $(FFT2MIDIDIR)/channelbroker.o

all: server

server: main.o $(SOCKET) $(FFT2MIDI) $(FFT2MIDIOBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) -g 

$(SOCKET):
	$(MAKE) -C ../socket

$(FFT2MIDI):
	$(MAKE) -C ../fft2midi

clean:
	-rm server
	-rm *.so
	-rm *.o
	$(MAKE) -C ../socket clean
	$(MAKE) -C $(FFT2MIDIDIR)/ clean
