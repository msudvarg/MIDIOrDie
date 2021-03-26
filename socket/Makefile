CPPFLAGS = -O2 -std=c++17 -fPIC
LIBS = -lrt
LIBTARGET = libsocket.a
all: base.o server.o client.o connection.o $(LIBTARGET)
base.o: base.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -c $^
server.o: server.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -c $^
client.o: client.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -c $^
connection.o: connection.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -c $^
$(LIBTARGET):
	$(AR) rcs $(LIBTARGET) *.o
clean:
	-rm *.o
	-rm *.a
