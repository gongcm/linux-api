## Netlink

Netlink 是内核空间和用户空间通信方式中的一种。

```
// linux/netlink.h 中定义了通信消息的类型

#define NETLINK_ROUTE       0   /* Routing/device hook              */
#define NETLINK_UNUSED      1   /* Unused number                */
#define NETLINK_USERSOCK    2   /* Reserved for user mode socket protocols  */
#define NETLINK_FIREWALL    3   /* Unused number, formerly ip_queue     */
#define NETLINK_SOCK_DIAG   4   /* socket monitoring                */
#define NETLINK_NFLOG       5   /* netfilter/iptables ULOG */
#define NETLINK_XFRM        6   /* ipsec */
#define NETLINK_SELINUX     7   /* SELinux event notifications */
#define NETLINK_ISCSI       8   /* Open-iSCSI */
#define NETLINK_AUDIT       9   /* auditing */
#define NETLINK_FIB_LOOKUP  10
#define NETLINK_CONNECTOR   11
#define NETLINK_NETFILTER   12  /* netfilter subsystem */
#define NETLINK_IP6_FW      13
#define NETLINK_DNRTMSG     14  /* DECnet routing messages */
#define NETLINK_KOBJECT_UEVENT  15  /* Kernel messages to userspace */
#define NETLINK_GENERIC     16
/* leave room for NETLINK_DM (DM Events) */
#define NETLINK_SCSITRANSPORT   18  /* SCSI Transports */
#define NETLINK_ECRYPTFS    19
#define NETLINK_RDMA        20
#define NETLINK_CRYPTO      21  /* Crypto layer */

#define NETLINK_INET_DIAG   NETLINK_SOCK_DIAG

#define MAX_LINKS 32
```



### Netlink 用户空间编程

```
//1.创建 socket
fd = socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT); // 热插拔事件消息

//2. 绑定bind到具体的进程
struct sockaddr_nl nl;

nl.nl_family = AF_NETLINK;
nl.nl_pid = getpid(); // 进程 pid
nl.nl_groups = 1; // 进程支持组

bind(fd,(struct sockaddr *)&nl,sizeof(struct sockaddr));

//3. read write 

char buffer[1024];
read(fd,buffer,sizeof(buffer));

```



### Netlink 内核空间编程

```

// linux/netlink.h

/******************************
net:    linux网络命名空间结构体指针
uint:   netlink协议类型
cfg:    netlink内核配置参数结构体指针
返回: sock结构指针
******************************/

static inline struct sock *
netlink_kernel_create(struct net *net, int unit, struct netlink_kernel_cfg *cfg);


```
