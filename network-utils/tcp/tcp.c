#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>

#include "tcp.h"
/*
    socket
    bind
    listen
    accept

 */

#ifdef TCP_DEBUG 
#define tcp_printf(format,args...) printf("%s [%d]"format"\n",__func__,__LINE__,##args)
#else
#define tcp_printf(format,args...) 
#endif

int tcp_socket()
{
  return socket(AF_INET,SOCK_STREAM,0);
}

int tcp_bind(int fd,char * ip,int port)
{
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  if(!ip){
    addr.sin_addr.s_addr   = htons(INADDR_ANY);
  }else{
    addr.sin_addr.s_addr = inet_addr(ip);
  }
  addr.sin_port   = htons(port);
  if(bind(fd,(struct sockaddr *)&addr,sizeof(struct sockaddr)) < 0){
    perror("tcp binder : ");
    return -1;
  }

  tcp_printf("2 Bind \n");
  return 0;
}

int tcp_listen(int fd,int max)
{
  if(listen(fd,max) < 0){
    perror("tcp listen : ");
    return -1;
  }

  return 0;
}

int tcp_accept(int fd,tcp_addr * addr)
{
  int ret = -1;
  socklen_t len = sizeof(struct sockaddr);
  
  struct sockaddr_in peer; 

  ret = accept(fd,(struct sockaddr *)&peer,&len);
  if(ret < 0 || len <= 0){
    perror("accept : ");
    return -1;
  }

  strcpy(addr->ip,inet_ntoa(peer.sin_addr));
  addr->port = ntohs(peer.sin_port);

  tcp_printf("Client addr : %s \n",addr->ip);
  tcp_printf("Client port : %d \n",addr->port);

  return ret;
}

int tcp_connect(int fd,tcp_addr * addr)
{
  int ret = -1;
  struct sockaddr_in s;

  s.sin_family = AF_INET;
  s.sin_addr.s_addr = inet_addr(addr->ip);
  s.sin_port = htons(addr->port);

  ret = connect(fd,(struct sockaddr *)&s,sizeof(struct sockaddr));
  if(ret < 0){
    perror("tcp connect : ");
    return -1;
  }

  tcp_printf("Server addr : %s \n",addr->ip);
  tcp_printf("Server port : %d \n",addr->port);

  return ret;
}


int tcp_read(int fd,char * buffer,int size)
{
  int ret = -1;
  int len = size;

  while(len > 0){
    ret = read(fd,buffer,len);
    if(ret <= 0){
      if(ret == -EINTR){
        ret = 0;
      }else{
        break;
      }
    }

    len -= ret;
    buffer += ret;
  }
  return (size - len);
}

int tcp_write(int fd,char * buffer,int size)
{
  int ret = -1;
  int left = size;
  while(left > 0){
    ret = write(fd,buffer,left);
    if(ret <= 0){
      if(ret == -EINTR){
        ret = 0;
      }else {
        break;
      }
    }

    left -= ret;
    buffer += ret;
  }

  return (size - left);
}


// timeout millseconds
int select_read(int fd,char * buffer,int bufsize,int timeout)
{
    int ret = 0;
    fd_set fds;
    struct timeval tval;


    if(fd < 0 || !buffer){
      tcp_printf("read fd %d,buffer %ld \n",fd,(long int)buffer);
      return -1;
    }

    //FD_CLR(fd,&fds);
    FD_ZERO(&fds);
    FD_SET(fd,&fds);
    
    tval.tv_sec = timeout / 1000;
    tval.tv_usec = timeout % 1000 * 1000;

    while((ret = select(fd+1,&fds,NULL,NULL,&tval)) > 0){
      if(!FD_ISSET(fd,&fds)){
        ret = - errno;
        tcp_printf("select error ...\n");
        break;
      }
      ret = tcp_read(fd,buffer,bufsize);
      break;
    }

    return ret;
}

int select_write(int fd ,char * buffer,int bufsize,int timeout)
{
  int ret = -1;
  fd_set fds;
  struct timeval tval;

  if(fd < 0 || !buffer){
    return -1;
  }

  FD_ZERO(&fds);
  FD_SET(fd,&fds);

  tval.tv_sec = timeout / 1000;
  tval.tv_usec = timeout %  1000 * 1000;
  while((ret = select(fd+1,NULL,&fds,NULL,&tval)) > 0){
    if(!FD_ISSET(fd,&fds)){
      tcp_printf("select write error ... \n");
      break;
    }

    ret = tcp_write(fd,buffer,bufsize);
    break;
  }

  return ret;
}

