#include <stdio.h>


#define TCP_SUCCESS 0
#define TCP_TIMEOUT -1 
#define TCP_FAILED  -2


typedef struct tcp_addr{
  char ip[32];
  int  port;
}tcp_addr;

int tcp_socket();

int tcp_bind(int fd,char * ip,int port);
int tcp_listen(int fd,int max);
int tcp_accept(int fd,tcp_addr * addr);
int tcp_connect(int fd,tcp_addr* addr);

int tcp_read(int fd,char * buffer,int bufsize);
int tcp_write(int fd,char * buffer,int bufsize);

int select_read(int fd,char * buffer,int bufsize,int timeout);
int select_write(int fd,char * buffer,int bufsize,int timeout);

