
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

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

    if(argc < 1){
        printf("usage：\n\t %s server ip port ",argv[0]);
        return 0;
    }

   // 1. socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("socket : ");
        return -1;
    }

    struct sockaddr_in  addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    addr.sin_port = htons(8899);

    // 2. bind address
    if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0){
        perror("Bind : ");
        return -2;
    }

    // 3 .listen
    if(listen(sockfd,5) < 0){
        perror("listen : ");
        return -3;
    }

    int client_fd;
    int count;
    struct sockaddr_in peer;
    socklen_t len;
    while(1){

        // 4. accept
        client_fd = accept(sockfd,(struct sockaddr *)&peer,&len);
        if(client_fd < 0){
            perror("accpet : ");
            break;
        } 
        //in_addr
        printf("ip %s  \n",inet_ntoa(peer.sin_addr));
        printf("port %d \n",ntohs(peer.sin_port));

        while(1){
            ret = recv(client_fd,&count,sizeof(count),0);
            printf("server recv,count %d,ret %d \n",count,ret);


            count = count + 1;

            ret = send(client_fd,&count,sizeof(count),0);
            printf("server send,count %d,ret %d \n",count,ret);

            if(count >= 5){
                //break;
                close(client_fd);
                printf("server exit ... \n");
                return 0;
            }
        }
        //close(client_fd);
       // ret = select(sockfd,&socks,NULL,NULL,)
    }

    // unreachable
    return 0;
}
