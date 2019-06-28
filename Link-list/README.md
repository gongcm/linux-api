# Linklist 
    
    sllist.c 实现了一个单向的link-list 数据结构。
    
    使用link list 实现消息队列，用于进程，线程间同步通信。
    
    - pthread_mutex_t 使用

        pthread_mutex_init()
        pthread_mutex_trylock()
        pthread_mutex_lock()
        pthread_mutex_unlock()
        pthread_mutex_destroy()

    - pthread_cond_t 使用

        pthread_cond_init()
        pthread_cond_wait()
        ptread_cond_timed_wait()
        pthread_cond_signal()
        pthread_cond_broadcast()
        pthread_cond_destoroy()
    
    pthread_mutex_t 和 pthread_cond_t 使用能做到线程之间的同步。

---
