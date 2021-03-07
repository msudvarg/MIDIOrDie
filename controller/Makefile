CPPFLAGS = -O2 -std=c++17 -Wall `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt -Lsocket -lsocket `pkg-config --libs portaudiocpp python3`
SWIG = swig
all: socket controller fft_histogram.so
socket: socket/libsocket.a
socket/libsocket.a:
	$(MAKE) -C socket
test: test.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS)
controller: main.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) 
fft_histogram.so: fft_histogram.cpp
	$(CXX) $(CPPFLAGS) -shared -fPIC -o $@ $^ $(LIBS)