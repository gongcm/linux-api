#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <limits.h>  // PATH_MAX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "unix_socket.h"

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

    if(argc <= 0){
        printf("usage：\n\t %s server socket path \n",argv[0]);
        return 0;
    }

    //printf("server addr : %s \n",argv[1]);
    printf("server path : %s \n",argv[1]);

   // 1. socket
   // 2. bind
   sockfd = unix_socket_open((char *)argv[1]);
    if(sockfd < 0){
        perror("socket : ");
        return -1;
    }

    // 3 .listen
    if(unix_listen(sockfd,5) < 0){
        perror("listen : ");
        return -3;
    }

    int client_fd;
    int count;
    char path[PATH_MAX];

    while(1){

        // 4. accept
        client_fd = unix_accept(sockfd,path);
        if(client_fd < 0){
            perror("accpet : ");
            break;
        } 
        //in_addr
        printf("client fd %d,path %s\n",client_fd,path);

        while(1){
            
            //ret = recv(client_fd,&count,sizeof(count),0);
            ret = unix_read(client_fd,(char *)&count,sizeof(count));
          
            if(ret == -1){ // timeout 
              continue;
            }
            if(ret < 0){
              printf("read failed \n");
              break;
            }

            printf("server recv,count %d,ret %d \n",count,ret);
            
            sleep(1);


            count = count + 1;

            ret = unix_write(client_fd,(char *)&count,sizeof(count));
            printf("server send,count %d,ret %d \n",count,ret);

            if(count >= 5){
                //break;
               close(client_fd);
               //unix_socket_close(sockfd);
                printf("server exit ... \n");
                return 0;
            }
            sleep(1);
        }
    }

    // unreachable
    return 0;
}
