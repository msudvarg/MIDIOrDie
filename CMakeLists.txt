cmake_minimum_required(VERSION 3.13)
Project(MidiOrDie)

set (CMAKE_CXX_STANDARD 17)

option(BUILD_TEST_PROGRAMS "Extra programs for testing" OFF)
option(BUILD_SERVER "Build the server (requires tensorflow)" OFF)

## Third party libraries
set(PA_DISABLE_INSTALL ON CACHE "" "" FORCE)
add_subdirectory(portaudio)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi_uninstall" CACHE "" "" FORCE)
add_subdirectory(rtmidi)

## MidiOrDie libraries
add_subdirectory(socket)
add_subdirectory(localcontroller)
if (BUILD_SERVER)
  add_subdirectory(fft2midi)
  add_subdirectory(cnpy)

  add_executable(server server/main.cpp)
  target_link_libraries(server
    socket
    cnpy
    fft2midi)
endif () ## BUILD_SERVER

add_executable(controller controller/main.cpp)
target_link_libraries(controller
  socket
  lc)

if (BUILD_TEST_PROGRAMS)
  add_subdirectory(fft)
  add_executable(test_fft2midi testprograms/test_fft2midi.cpp)
  target_link_libraries(test_fft2midi
    socket
    fft
    lc
    fft2midi)

  add_executable(channelbroker_test testprograms/channelbroker_test.cpp)
  target_link_libraries(channelbroker_test
    fft2midi)
endif ()
