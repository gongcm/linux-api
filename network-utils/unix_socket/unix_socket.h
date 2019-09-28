#include <stdio.h>


int unix_socket_open(char * path);
int unix_socket_close(int fd);

int unix_listen(int fd,int max);
int unix_accept(int fd,char * path);
int unix_connect(int fd,char * path);

int unix_read(int fd,char * buffer,int bufsize);
int unix_write(int fd,char * buffer,int bufsize);
