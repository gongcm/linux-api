#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tcp.h"

/*
    socket
    bind
    listen
    accept

 */

int main(int argc, char const *argv[])
{
    int sockfd = -1;
    int ret = -1;

    if(argc <= 1){
        printf("usage：\n\t %s server port \n",argv[0]);
        return 0;
    }

    //printf("server addr : %s \n",argv[1]);
    printf("server port : %d \n",atoi(argv[1]));

   // 1. socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("socket : ");
        return -1;
    }

    // 2. bind address
    if(tcp_bind(sockfd,NULL,atoi(argv[1]))< 0){
        perror("Bind : ");
        return -2;
    }

    // 3 .listen
    if(tcp_listen(sockfd,5) < 0){
        perror("listen : ");
        return -3;
    }

    int client_fd;
    int count;
    tcp_addr addr;
    while(1){

        // 4. accept
        client_fd = tcp_accept(sockfd,&addr);
        if(client_fd < 0){
            perror("accpet : ");
            break;
        } 
        //in_addr
        printf("ip %s  \n",addr.ip);
        printf("port %d \n",addr.port);

        while(1){
            
            //ret = recv(client_fd,&count,sizeof(count),0);
            ret = tcp_read(client_fd,(char *)&count,sizeof(count));
            
            if(ret == -1){ // timeout 
              continue;
            }
            if(ret < 0){
              printf("read failed \n");
              break;
            }

            printf("server recv,count %d,ret %d \n",count,ret);


            count = count + 1;

            ret = tcp_write(client_fd,(char *)&count,sizeof(count));
            printf("server send,count %d,ret %d \n",count,ret);

            if(count >= 5){
                //break;
                close(client_fd);
                printf("server exit ... \n");
                return 0;
            }
        }
    }

    // unreachable
    return 0;
}
