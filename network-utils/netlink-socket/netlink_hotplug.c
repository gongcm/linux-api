#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
// hotplug event test 

int main()
{
  int fd = -1;
  struct sockaddr_nl nl; // need ot bind a process

  fd = socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT);
  if(fd < 0){
    printf("Netlink socket create failed \n");
    
    return -1;
  }

  nl.nl_family = AF_NETLINK;
  nl.nl_pid = getpid();
  nl.nl_groups = 1;

  if(bind(fd,(struct sockaddr*)&nl,sizeof(struct sockaddr)) < 0){
     perror("Netlink bind");
     return -1;
  }

  int ret = -1;
  char buffer[1024];
  while(1){
    /*
     * 内核中使用下面函数发送消息到用户空间
     * int netlink_unicast(struct sock *ssk, struct sk_buff *skb,u32 pid, int nonblock)
     * int netlink_broadcast(struct sock *ssk, struct sk_buff *skb, u32 pid,u32 group, gfp_t allocation)
     * */
    ret = read(fd,buffer,sizeof(buffer));
    printf("KOBJECT_UEVENT : %s \n",buffer);
    sleep(1);
  }

  return 0;
}

