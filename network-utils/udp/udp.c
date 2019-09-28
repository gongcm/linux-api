#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include "udp.h"

#ifdef UDP_DEBUG
#define udp_printf(format,args...) printf("%s[%d] "format"\n",__func__,__LINE__,## args)
#else
#define udp_printf(format,args...)
#endif

int udp_socket(){
  return socket(AF_INET,SOCK_DGRAM,/*IPPROTO_UDP*/0);
}

int udp_bind(int fd,udp_addr * addr)
{
  struct sockaddr_in s; 

  s.sin_family = AF_INET;
  // Just bind in udp_server so that we can recv all message from all inet_addr on udp port.
  s.sin_addr.s_addr = inet_addr(addr->ip);
  s.sin_port  = htons(addr->port);

  if(bind(fd,(struct sockaddr*)&s,sizeof(struct sockaddr)) < 0){
    perror("udp bind");
    return -1;
  }

  udp_printf("2 udp bind \n");
  return 0;
}

int udp_send(int fd,char * buffer,int bufsize,udp_addr * addr)
{
  int ret = -1;
  int left = bufsize;
  socklen_t len = sizeof(struct sockaddr);
  struct sockaddr_in s; 

  s.sin_family = AF_INET;
  s.sin_addr.s_addr = inet_addr(addr->ip);
  s.sin_port  = htons(addr->port);

  while(left > 0){
    ret = sendto(fd,buffer,left,0,(struct sockaddr *)&s,len);
    if(ret < 0){
      if(errno == EINTR){
        ret = 0;
      }else{
        perror("sendto");
        return ret;
      }
    }

    buffer += ret;
    left -= ret;
  }

  return bufsize - left;
}

int udp_recv(int fd ,char * buffer,int bufsize,udp_addr * addr)
{
  int ret = -1;
  int len = 0;
  int left = bufsize;
  struct sockaddr_in s;

  ret = recvfrom(fd,buffer,bufsize,0,(struct sockaddr *)&s,&len);
  if(ret < 0){
    perror("recvfrom");
  }
  
  strcpy(addr->ip,inet_ntoa(s.sin_addr));
  addr->port = ntohs(s.sin_port);

  return ret;
}

int udp_recv_timeout(int fd,char * buffer,int bufsize,udp_addr * addr,int timeout)
{
  int ret = 0;
  fd_set fds;
  struct timeval tval;

  FD_ZERO(&fds);
  FD_SET(fd,&fds);

  tval.tv_sec = timeout / 1000;
  tval.tv_usec = timeout % 1000 * 1000;

  while((ret = select(fd+1,&fds,NULL,NULL,&tval)) >0 ){
    if(FD_ISSET(fd,&fds)){
      ret = udp_recv(fd,buffer,bufsize,addr);
      if(ret < 0){
        ret = -110;
      }
      break;  
    }
  }
  return ret;
}
