
all: clean 
	gcc server.c unix_socket.c -I. -o server
	gcc client.c unix_socket.c -I. -o client

clean:
	rm -rf server
	rm -rf client

