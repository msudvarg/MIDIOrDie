CPPFLAGS = -O2 -std=c++17 -Wall `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt -Lsocket -lsocket `pkg-config --libs portaudiocpp python3`
SWIG = swig
all: socket test fft_histogram.so
socket: socket/libsocket.a
socket/libsocket.a:
	$(MAKE) -C socket
test: test.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS)
controller: main.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) 
fft_histogram.so: fft_histogram.cpp socket/base.cpp socket/client.cpp socket/server.cpp socket/connection.cpp
	$(CXX) $(CPPFLAGS) -shared -fPIC -o $@ $^ $(LIBS)
clean:
	-rm test
	-rm *.so
	-rm *.o
	$(MAKE) -C socket clean
