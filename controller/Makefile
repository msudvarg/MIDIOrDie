CPPFLAGS = `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt `pkg-config --libs portaudiocpp python3`
all: controller fft_histogram.so
controller: main.o
	$(CXX) $(LIBS) -o $@ $^
fft_histogram.so: fft_histogram.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -shared -fPIC -o $@ $^
