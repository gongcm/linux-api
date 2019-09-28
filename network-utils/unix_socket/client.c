#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "unix_socket.h"

struct msg{
    int count;
};

int main(int argc, char const *argv[])
{
    int sockfd = -1;
    int ret = -1;

    if(argc <= 1){
        printf("Usage : \n"
                "\t %s server path client path\n",argv[0]
            );
            return -1;
    }

    printf("server path %s client path %s \n",argv[1],argv[2]);
    // tcp socket fd

   // 1. create unix socket
   // 2. unix bind

  sockfd = unix_socket_open((char *)argv[2]);
  if(sockfd < 0){
    perror("unix_socket_open");
    return -1;
  }

  //3 . connect unix server.
  if(ret = unix_connect(sockfd,(char *)argv[1]) < 0){
    perror("connect : ");
    return -2;
  }

  int count = 0;
   
  while(1){
        ret = unix_write(sockfd,(char *)&count,sizeof(count));
        if(ret < 0){
            perror("send : ");
            break;
        }

        printf("client send len %d,count %d \n",ret,count);

        count = 0;
        ret = unix_read(sockfd,(char *)&count,sizeof(count));
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
       // close(sockfd);
       unix_socket_close(sockfd);
    }

    printf("client eixt ... \n");
    return 0;
}
