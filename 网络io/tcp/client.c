#include <stdio.h>
#include <stdlib.h> // atoi
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

struct msg{
    int count;
};

int main(int argc, char const *argv[])
{
    int sockfd = -1;
    struct sockaddr_in addr;
    socklen_t  socklen = sizeof(addr);
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
    memset(&addr,0,sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port   = htons(atoi(argv[2]));
    
    #if 0 
    // tcp 客户端 可以不使用 bind
    if(ret = bind(sockfd,(struct sockaddr *)&addr,socklen) < 0){
        perror("bind : ");
        return -1;
    }
    #endif 

    if(ret = connect(sockfd,(struct sockaddr *) &addr,socklen) < 0){
        perror("connect : ");
        return -2;
    }

    int count = 0;
    while(1){
        ret = send(sockfd,(void *)&count,sizeof(count),0);
        if(ret < 0){
            perror("send : ");
            break;
        }

        printf("client send ,len %d,count %d \n",ret,count);

        count = 0;
        ret = recv(sockfd,&count,sizeof(count),0);
        if(ret < 0){
            perror("recv: ");
            break;
        }
        printf("client recv,len %d,count %d \n",ret,count);       
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
