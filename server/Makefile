CPPFLAGS = -O2 -Wall `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt -L../socket -lsocket `pkg-config --libs portaudiocpp python3`
SWIG = swig
all: server
server: main.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIBS) -g 
clean:
	-rm server
	-rm *.so
	-rm *.o
