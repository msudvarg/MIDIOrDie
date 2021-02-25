CPPFLAGS = -g `pkg-config --cflags fftw3 portaudiocpp python3` -fPIC
LIBS = -lm `pkg-config --libs fftw3 portaudiocpp python3`

all: driver localcontroller.py _localcontroller.so

driver: driver.o localcontroller.o
	$(CXX) $(LIBS) -o $@ $^

localcontroller.py localcontroller_wrap.cpp: localcontroller.i
	swig -c++ -python -o localcontroller_wrap.cpp localcontroller.i

_localcontroller.so: localcontroller_wrap.o localcontroller.o
	$(CXX) $(LIBS) -shared -o $@ $^
