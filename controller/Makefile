CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp python3`
SWIG = swig
all: controller test fft_histogram.so
controller: main.o fft.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) -g 
test: test.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS)
fft_histogram.so: fft_histogram.cpp
	$(CXX) $(CPPFLAGS) -shared -fPIC -o $@ $^ $(LIBS)
clean:
	-rm test
	-rm controller
	-rm *.so
	-rm *.o
