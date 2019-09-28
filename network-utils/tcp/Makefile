
all: clean 
	gcc server.c tcp.c -I. -o server
	gcc client.c tcp.c -I. -o client

clean:
	rm -rf server
	rm -rf client

