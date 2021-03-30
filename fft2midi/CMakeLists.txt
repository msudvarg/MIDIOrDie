Project(MidiOrDie)

add_library(fft2midi SHARED
  channelbroker.cpp
  channelbroker.h
  fft2midi.cpp
  fft2midi.h
  midi.cpp
  midi.h
  tone.cpp
  tone.h)

target_link_libraries(fft2midi
  rtmidi
  portaudio)
