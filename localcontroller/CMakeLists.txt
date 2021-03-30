Project(MidiOrDie)

add_library(lc
  localcontroller.cpp
  localcontroller.h)

target_link_libraries(lc
  fftw3)
