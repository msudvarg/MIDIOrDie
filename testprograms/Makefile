CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp python3 rtmidi`
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp python3 rtmidi`
SWIG = swig
FFT = ../fft/fft.o
SOCKET = ../socket/libsocket.a
FFT2MIDIDIR = ../fft2midi
FFT2MIDIOBJS = $(FFT2MIDIDIR)/midi.o $(FFT2MIDIDIR)/tone.o $(FFT2MIDIDIR)/fft2midi.o

all: testsocket test_fft2midi visualizer

testsocket: testsocket.o  $(SOCKET) $(FFT)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS)

test_fft2midi: test_fft2midi.o $(FFT) $(FFT2MIDIOBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS)

visualizer: fft_histogram.so

fft_histogram.so: fft_histogram.cpp $(SOCKET)
	$(CXX) $(CPPFLAGS) -shared -fPIC -o $@ $^ $(LIBS)

$(SOCKET):
	$(MAKE) -C ../socket
clean:
	-rm testsocket
	-rm test_fft2midi
	-rm *.so
	-rm *.o
