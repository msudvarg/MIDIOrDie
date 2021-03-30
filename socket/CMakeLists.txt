Project(MidiOrDie)

add_library(socket SHARED
  base.cpp
  base.h
  client.cpp
  client.h
  connection.cpp
  connection.h
  server.cpp
  server.h
  socket.h)

target_link_libraries(socket rt)
