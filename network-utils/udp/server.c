#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "udp.h"

int main(int argc,char * argv[])
{
  int fd = -1;
  int ret = -1;

  udp_addr addr;
  if(argc <= 1){
    printf("usage : \n\t%s ip port \n",argv[0]);
    exit(0);
  }

  printf("ip %s port %s \n",argv[1],argv[2]);

  strcpy(addr.ip,argv[1]);
  addr.port = atoi(argv[2]);

  fd = udp_socket(); 
  if(fd < 0){
    perror("udp_socket");
    return -1;
  }

  printf("socket fd %d \n",fd);

  ret = udp_bind(fd,&addr);
  if(ret < 0){
    perror("udp_bind");
    return -2;
  }

  int count = 0;
  udp_addr caddr;
  while(1){
    ret = udp_recv(fd,(char *)&count,sizeof(count),&caddr);
    if(ret < 0){
      perror("udp_recv");
      break;
    }
    printf("server udp_recv ip %s,port %d,count %d,ret %d \n",caddr.ip,caddr.port,count,ret);

    count = count + 1;
    
    usleep(500*1000);
    ret = udp_send(fd,(char *)&count,sizeof(count),&caddr);
    printf("udp_send ret %d \n",ret);
    if(count == 5){
      break;
    }
  }

  printf("server exit ... \n");
  if(fd)
    close(fd);

  return 0;
}
