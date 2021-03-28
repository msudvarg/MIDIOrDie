CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp`
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp`
FFT = ../fft/fft.o
SOCKET = ../socket/libsocket.a

all: controller

controller: main.o $(SOCKET) $(FFT)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) -g 

$(FFT):
	$(MAKE) -C ../fft

$(SOCKET):
	$(MAKE) -C ../socket

clean:
	-rm test
	-rm controller
	-rm *.so
	-rm *.o
	$(MAKE) -C ../socket clean
	$(MAKE) -C ../fft clean