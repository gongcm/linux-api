
all: clean 
	gcc server.c udp.c -I. -o server
	gcc client.c udp.c -I. -o client

clean:
	rm -rf server
	rm -rf client

