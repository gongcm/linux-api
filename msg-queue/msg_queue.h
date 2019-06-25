#include <stdio.h>
#include <pthread.h>

//typedef void * Msg_header;
typedef struct Msg_header{
    int size; // queue size
    int entry_size; // msg enrty size
    int num;
    void * ptr; // buffer
    int r,w; // 读写指针
    pthread_mutex_t * mutex;
    pthread_cond_t * bRead;
    pthread_cond_t * bWrite;
}MsgHeader;

MsgHeader * msg_queue_init(int entry_num,int entry_size);
int msg_put(MsgHeader * h,void * msg,int size);
int msg_get(MsgHeader* h,void * msg,int size);
int msg_queue_release(MsgHeader * h);