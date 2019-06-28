# Linux 文件设计

- linux是一切皆为文件思想设计的。

    在Linux系统中文件分为：普通文件、目录文件、字符设备文件、块设备文件、链接文件、管道文件、网络文件。
    对于这7种文件类型Linux内核中用统一的结构体struct file 去描述。然后使用RBtree对文件进行管理。

    在内核中使用struct file结构体进行描述：
        
        struct file {
                /*
                 * fu_list becomes invalid after file_free is called and queued via
                 * fu_rcuhead for RCU freeing
                 */
                
                union {
                    struct list_head    fu_list;
                    struct rcu_head     fu_rcuhead; // RCU 机制
                } f_u;
                
                struct path        f_path; // 文件路径对象
                
                #define f_dentry    f_path.dentry   //该成员是对应的 目录结构 。
                #define f_vfsmnt    f_path.mnt      
                
                const struct file_operations    *f_op;  //该操作 是定义文件关联的操作的。内核在执行open时对这个 指针赋值。
                
                atomic_long_t        f_count;  // 引用次数
                unsigned int         f_flags;  //该成员是文件标志。 
                mode_t            f_mode; // 文件类型    
                loff_t            f_pos;
                
                struct fown_struct    f_owner;  // 用户
                unsigned int        f_uid, f_gid;
                
                struct file_ra_state    f_ra;

                u64            f_version;
                
                #ifdef CONFIG_SECURITY
                void            *f_security;
                #endif
                
                /* needed for tty driver, and maybe others */
                void            *private_data;//该成员是系统调用时保存状态信息非常有用的资源。

                #ifdef CONFIG_EPOLL
                
                /* Used by fs/eventpoll.c to link all the hooks to this file */
                struct list_head    f_ep_links;
                spinlock_t        f_ep_lock;
                
                #endif /* #ifdef CONFIG_EPOLL */
                
                struct address_space    *f_mapping; // 映射的虚拟地址空间
                
                #ifdef CONFIG_DEBUG_WRITECOUNT
                unsigned long f_mnt_write_state;
                #endif
        }

    struct file 结构体描述了一个文件该有的哪些属性： 文件读写、执行、文件路径、文件被引用的次数、文件被映射的虚拟地址。而对于这些属性linux在设计时基于虚拟文件系统向用户层提供了很多API接口：

    文件：
        #include <fcntl.h>
            open    文件打开
            fcntl 函数 文件读写、执行等属性 操作
            creat 创建文件
            unlink
        
        #include <unistd.h>
            read    // 读取
            write   // 写
            close   // 关闭
            lseek   // 偏移

        sync
        fsync
        fdatasync

    对于目录文件
            opendir
            readdir
            closedir

            mkdir
            rmdir
            
            stat
            lstat
            fstat

            umask 正对于每个进程创建文件时，设置权限屏蔽值
            chdir

            chmod
            chown
    
    标准文件IO： 
        
        setbuf(FILE *fp,char * restrict buffer); // buffer 为NULL关闭缓冲
        1. IO流思想： 字符流、二进制流
        2.在设计时都回附带一个缓冲区。

# 文件系统设计

    文件系统设计是为了对文件进行管理，其实文件管理涉及到磁盘管理。

    虚拟文件系统设计是为了提高多中文件系统的兼容性，对应用文件操作提供统一的API接口。

# 内存文件系统

    个人觉得内存文件系统的设置是为了驱动调试而设计，比如所有的驱动都可以在sysfs、procfs内存文件系统中导出一些参数信息，以便在调试驱动时，能够更快的修改硬件参数。

    内存文件系统中文件操作就是对一块内存进行直接操作。

# Linux 系统配置文件
   
    #include <pwd.h>
        /etc/passwd 
        /etc/shadow

    #include<netdb.h>
    // struct hostent   /etc/hosts          getnameinfo         getaddrinfo
    // struct netent    /etc/networks       getnetbyname        getnetbyaddr
    // struct protent   /etc/protocols      Getprotobyname      getprotobynumber
    // struct servent   /etc/services       getservbyname       getservbyport
     