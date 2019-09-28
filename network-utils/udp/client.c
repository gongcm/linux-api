#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "udp.h"

int main(int argc,char *argv[])
{
  int fd = -1;
  udp_addr addr;
  int ret = -1;
  int port = 0;
  if(argc <= 2){
    printf("usage : \n\t %s ip port \n",argv[0]);
    return -1;
  }

  strcpy(addr.ip,argv[1]);
  addr.port = atoi(argv[2]);
  
  //port = addr.port + 1;

  printf("server ip %s,port %d \n",argv[1],atoi(argv[2]));

  fd = udp_socket();
  if(fd < 0){
    perror("udp_socket");
    return -1;
  }

  printf("socket fd %d \n",fd);

  //addr.port = port;
  //if(udp_bind(fd,&addr) < 0){
  //  perror("udp_bind");
  //  return -1;
  //}


  //addr.port = port -1;
  int count = 0;
  udp_addr sc;
  while(1){
    ret = udp_send(fd,(char *)&count,sizeof(count),&addr); 
    if(ret < 0){
      perror("udp_send");
      break;
    }
    printf("udp_send ret %d \n",ret);
    ret = udp_recv(fd,(char *)&count,sizeof(count),&sc);
    printf("udp_recv ip %s,port %d,count %d,ret %d \n",sc.ip,sc.port,count,ret);

    if(count == 5) break;
  }

  printf("client exit ...\n");
  close(fd);
  return 0;
}
