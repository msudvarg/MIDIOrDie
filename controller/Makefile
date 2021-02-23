CPPFLAGS = `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt -lportaudio -pthread `pkg-config --libs portaudiocpp python3`
all: controller fft_histogram.so
controller: main.cpp
	$(CXX) $^ $(LIBS) -o $@
fft_histogram.so: fft_histogram.cpp
	$(CXX) $^ $(CPPFLAGS) $(LIBS) -shared -fPIC -o $@
clean:
	rm controller fft_histogram.so