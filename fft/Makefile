CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp`
LIBS = -lrt `pkg-config --libs portaudiocpp`

all: fft.o

fft:
	$(CXX) $(CPPFLAGS) $@ $^ $(LIBS) -g

clean:
	-rm *.so
	-rm *.o