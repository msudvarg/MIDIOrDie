cmake_minimum_required(VERSION 3.13)
Project(MidiOrDie)

option(BUILD_TEST_PROGRAMS "Extra programs for testing" OFF)

## Third party libraries
set(PA_DISABLE_INSTALL ON CACHE "" "" FORCE)
add_subdirectory(portaudio)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi_uninstall" CACHE "" "" FORCE)
add_subdirectory(rtmidi)

## MidiOrDie libraries
add_subdirectory(socket)
add_subdirectory(fft2midi)
add_subdirectory(fft)
add_subdirectory(localcontroller)

add_executable(server server/main.cpp)
target_link_libraries(server
  socket
  fft2midi)

add_executable(controller controller/main.cpp)
target_link_libraries(controller
  socket
  fft
  lc)
