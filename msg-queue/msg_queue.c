#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include "msg_queue.h"

/*
    消息队列实现： 使用了内核中循环队列(ringbuffer)的思路
    
    如果一个线程在执行是没有到达pthread_cond_wait 代码时，另外一个线程已经执行pthread_cond_signal
    ，此时会导致 pthread_cond_wait 永远处于等待状态。

    解决方法：
        1. 可以在pthread_cond_signal 之后加一定的延时。
        2. 多调用几次pthread_cond_signal

*/

//  num :  msg num
//  entry  ： msgsize
//   return msg——queue-header
MsgHeader * msg_queue_init(int num,int entry){
    
    int size = num * entry;
    MsgHeader * h = NULL;

    size = size / entry * entry;

    h =(MsgHeader *)malloc(sizeof(MsgHeader));

    memset(h,0,sizeof(MsgHeader));

    if(!h->ptr) h->ptr = malloc(size);
    if(!h->mutex) h->mutex =(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(!h->bRead) h->bRead =(pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    
    h->bWrite = malloc(sizeof(pthread_cond_t));
    pthread_mutex_init(h->mutex,NULL);
    pthread_cond_init(h->bRead,NULL);
    pthread_cond_init(h->bWrite,NULL);

    h->size = size;
    h->entry_size = entry;
    h->num = h->size / h->entry_size;

    h->w = h->r = 1;

    return h;
}


int msg_put(MsgHeader * h,void * buffer,int size)
{
    int ret = -1;
    int offset = 0;
    if(!h|| !h->mutex || !h->bWrite)
        return -1;

    if(h->entry_size != size){
        printf("Waring msg size err \n");
        return -1;
    }


    pthread_mutex_lock(h->mutex);
    pthread_cleanup_push(pthread_mutex_unlock,h->mutex);
    while(1){
        offset = h->w - h->r;
        if(offset >= h->num) // 队列满了
        {
            //等待
            ret = pthread_cond_wait(h->bWrite,h->mutex);
            if(ret) break;
        }else{
            memcpy(h->ptr+(h->w - 1)* h->entry_size,buffer,size);

            h->w = h->w + 1;
            h->w = h->w % h->num;
            pthread_cond_signal(h->bRead);
            break;
        }
        
    }
    pthread_mutex_unlock(h->mutex);
    pthread_cleanup_pop(0);

    return ret > 0 ? ret : size;
}

int msg_get(MsgHeader*h,void * buff,int size){
    int ret = -1;
    int offset = -1;

    if(!h || !h->mutex || !h->bRead || !h->ptr){
        return -1;
    }

    // 每次获取的大小必须位定义的h->entry_size
    if(size > h->entry_size) return -2;

    pthread_mutex_lock(h->mutex);
    pthread_cleanup_push(pthread_mutex_unlock,h->mutex);
    while(1){

        offset = h->w - h->r;
        if(offset == 0){
            // 队列空了
            ret = pthread_cond_wait(h->bRead,h->mutex);
            if(ret) break;
        }else{
            memcpy(buff,h->ptr+(h->r-1) * h->entry_size,size);
            h->r = h->r + 1;
            h->r = h->r % h->num;
            pthread_cond_signal(h->bWrite);
            break;
        }
    }
    pthread_mutex_unlock(h->mutex);
    pthread_cleanup_pop(0);

    return ret >0 ? ret : size;
}


void msg_queue_release(MsgHeader * h)
{
    if(h){
        if(h->bRead){
            pthread_cond_destroy(h->bRead);
            free(h->bRead);
        } 

        if(h->bWrite){
             pthread_cond_destroy(h->bWrite);
             free(h->bWrite);
        }
        if(h->mutex) free(h->mutex);
        if(h->ptr) free(h->ptr);

        free(h);
    }

    //return 0;
}