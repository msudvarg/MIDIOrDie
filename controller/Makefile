CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp` #-Ilocalcontroller
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp` -L../fftw3 -llc -lfftw3 -Wl,-R,localcontroller
FFT = ../fft/fft.o
FFTW3 = ../fftw3/liblc.so
SOCKET = ../socket/libsocket.a

all: controller

controller: main.o $(SOCKET) $(FFTW3)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) -g 

$(FFT):
	$(MAKE) -C ../fft

$(FFTW3):
	$(MAKE) -C ../fftw3 liblc.so

$(SOCKET):
	$(MAKE) -C ../socket

clean:
	-rm test
	-rm controller
	-rm *.so
	-rm *.o
	$(MAKE) -C ../socket clean
	$(MAKE) -C ../fft clean