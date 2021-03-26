all: socket controller server

socket: ../socket/libsocket.a
../socket/libsocket.a:
	$(MAKE) -C socket

controller: controller/main.o controller/fft.o
	$(MAKE) -C controller

server: server/main.o
	$(MAKE) -C server


clean:
	-rm *.so
	-rm *.o
	$(MAKE) -C socket clean
	$(MAKE) -C controller clean