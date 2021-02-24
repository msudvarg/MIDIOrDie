CPPFLAGS = -O2 `pkg-config --cflags portaudiocpp python3`
LIBS = -lrt `pkg-config --libs portaudiocpp python3`
SWIG = swig
all: controller fft_histogram.so controller.py _controller.so
controller: main.o
	$(CXX) $(LIBS) -o $@ $^
fft_histogram.so: fft_histogram.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -shared -fPIC -o $@ $^
_controller.so: wrap_controller.o tone.o
	$(CXX) $(LIBS) -shared -o $@ $^
tone.o: tone.cpp
	$(CXX) $(CPPFLAGS) -fPIC -c $^
controller.py wrap_controller.o: controller.i
	$(SWIG) -o wrap_controller.cpp -c++ -python $<
	$(CXX) $(CPPFLAGS) -fPIC -c wrap_controller.cpp
