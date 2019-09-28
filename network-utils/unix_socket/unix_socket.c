#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <errno.h>

#ifdef UNIX_DEBUG
#define unix_printf(format,args...) printf("%s[%d] "format" \n",__func__,__LINE__,## args)
#else
#define unix_printf(format,args...)
#endif
static int unix_socket()
{
  return socket(AF_UNIX,SOCK_STREAM,0);
}

static int unix_bind(int fd,char * path)
{
  struct sockaddr_un addr;
  int len = 0;
  if(!access(path,0)){
    unlink(path);
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path,path);
  len = SUN_LEN(&addr);
  if(bind(fd,(struct sockaddr *)&addr,len) < 0){
    return -1;
  }

  return 0;
}

int unix_socket_open(char * path)
{
  int fd = unix_socket();

  if(fd < 0){
    perror("unix_socket");
    return -1;
  }

  if(path && unix_bind(fd,path) < 0){
    perror("unix_bind");
    return -1;
  }
  return fd;
}

int unix_socket_close(int fd){
  char file[PATH_MAX];
  char buffer[1024]= {'\0'};
  if(fd){
    snprintf(buffer,sizeof(buffer),"/proc/self/fd/%d",fd);
    readlink(buffer,file,sizeof(file)-1);
    unix_printf("file path %s \n",file);
    if(!access(file,0)){
      //remove()
      unlink(file);
    }
    close(fd);
  }
  return 0;
}


int unix_listen(int fd,int max)
{
  return listen(fd,max);
}

int unix_accept(int fd,char * path)
{
  int cfd = -1;
  struct sockaddr_un addr;
  int len = 0;

  if((cfd = accept(fd,(struct sockaddr *)&addr,&len)) < 0){
    return -1;
  }

  strncpy(path,addr.sun_path,strlen(path));

  return cfd;
}

int unix_connect(int fd,char * path)
{
  int ret = 0;
  int len = 0;
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path,path);

  // if gcc not surpport  -std=gnu99, SUN_LEN will be not defined.
  // SUN_LEN defined in /sys/un.h
  len = SUN_LEN(&addr);

  if((ret = connect(fd,(struct sockaddr *)&addr,len)) < 0){
    return -1;
  }

  return ret;
}

int unix_read(int fd,char * buffer,int size)
{
  int ret = -1;
  int len = size;

  while(len > 0){
    ret = recv(fd,buffer,len,0);
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

int unix_write(int fd,char * buffer,int size)
{
  int ret = -1;
  int left = size;
  while(left > 0){
    ret = send(fd,buffer,left,0);
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

