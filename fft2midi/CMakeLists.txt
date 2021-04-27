Project(MidiOrDie)

find_package(TensorflowCC REQUIRED)

add_library(fft2midi SHARED
  channel.h
  channel.cpp
  channelbroker.cpp
  channelbroker.h
  fft2midi.cpp
  fft2midi.h
  midi.cpp
  midi.h
  tone.cpp
  tone.h
  sigextr.cpp
  sigextr.h
  )

# link_libraries(tensorflow tensorflow_framework)

target_link_libraries(fft2midi
  rtmidi
  portaudio
  TensorflowCC::TensorflowCC
  protobuf)
