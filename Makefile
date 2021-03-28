all: controller server

controller: controller/main.o
	$(MAKE) -C controller

server: server/main.o
	$(MAKE) -C server

clean:
	-rm *.so
	-rm *.o
	$(MAKE) -C controller clean
	$(MAKE) -C server clean
	$(MAKE) -C testprograms clean
