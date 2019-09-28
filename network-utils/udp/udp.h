#include <stdio.h>

typedef struct udp_addr_S{

  char ip[32];
  int port;
}udp_addr;


int udp_socket();
int udp_bind(int fd,udp_addr * addr);
int udp_send(int fd,char * buffer,int bufsize,udp_addr * addr);
int udp_recv(int fd,char * buffer,int bufsize,udp_addr * addr);
int udp_recv_timeout(int fd,char * buffer,int bufsize,udp_addr * addr,int timeout);
