# linux-api
---
Linux api测试代码
---
### 1.MSG QUEUE
    msg-queue 实现了消息队列，使用了pthread_mutex_t、pthread_cond_t，同时封装了 linux 信号量、共享内存，文件锁。
    ```
        fcntl(int fd,int cmd,int flag);
        F_SETLKW // 阻塞
        F_SETLK  // 非阻塞
    ```
---
