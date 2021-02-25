CPPFLAGS = -g `pkg-config --cflags fftw3 portaudiocpp`
LIBS = -lm `pkg-config --libs fftw3 portaudiocpp`

driver: driver.o localcontroller.o
	$(CXX) $(LIBS) -o $@ $^
