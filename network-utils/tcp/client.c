#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "tcp.h"

struct msg{
    int count;
};

int main(int argc, char const *argv[])
{
    int sockfd = -1;
    int ret = -1;

    if(argc < 1){
        printf("Usage : \n"
                "\t %s ip port \n",argv[0]
            );
            return -1;
    }

    printf("ip %s port %s \n",argv[1],argv[2]);
    // tcp socket fd

   // 1. create socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("socket : ");
        return -1;
    }

  // 2 connect 
  tcp_addr addr; 

  strcpy(addr.ip,argv[1]);
  addr.port = atoi(argv[2]);

  if(ret = tcp_connect(sockfd,&addr) < 0){
    perror("connect : ");
    return -2;
  }

  int count = 0;
   
  while(1){
        ret = tcp_write(sockfd,(char *)&count,sizeof(count));
        if(ret < 0){
            perror("send : ");
            break;
        }

        printf("client send len %d,count %d \n",ret,count);

        count = 0;
        ret = tcp_read(sockfd,(char *)&count,sizeof(count));
        if(ret < 0){
            perror("recv: ");
            break;
        }
        printf("client recv len %d,count %d \n",ret,count);       
        if(count == 5){
            break;
        }
        sleep(1);
    }

    if(sockfd){
        close(sockfd);
    }

    printf("client eixt ... \n");
    return 0;
}
