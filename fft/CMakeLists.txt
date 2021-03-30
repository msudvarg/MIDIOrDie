Project(MidiOrDie)

add_library(fft
  fft.h
  fft.cpp)

target_link_libraries(fft
  portaudio)
