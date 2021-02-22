CPPFLAGS = `pkg-config --cflags portaudiocpp`
LIBS = -lrt `pkg-config --libs portaudiocpp`
all: controller
controller: main.o
	$(CXX) $(LIBS) -o $@ $^
