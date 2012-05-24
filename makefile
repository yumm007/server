OBJECTS = t01_server_main.o apply_config.o send_recv.o get_mac.o
CFLAGS = -g -Wall

server:	$(OBJECTS)
	cc $(OBJECTS) -o server

clean:
	@rm *.o *.log server send_cnf -rf

