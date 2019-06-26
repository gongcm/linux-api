#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "sllist.h"

//pthread_mute_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct MSgQueue
{
	SL_LIST head;
	SL_LIST free;

	int size;
	int msg_size;

	char * ptr;

	pthread_mutex_t mutex;
	pthread_cond_t bRead;
	pthread_cond_t bWrite;
	/* data */
}MSG_QUEUE;


typedef struct data{
	SL_NODE node;
	int entry_size;
}MSG_NODE;

#define MSG_NODE_SIZE (sizeof(MSG_NODE))

MSG_QUEUE * msg_queue_init(int msg_nums,int entry_size)
{
	int i = 0;

	MSG_QUEUE * p = NULL;
	char * data;
	int size = 0;

	entry_size  = entry_size + MSG_NODE_SIZE;
	
	size = entry_size * msg_nums;

	size = size / entry_size * entry_size;
	//SL_NODE * node = malloc(sizeof(SL_NODE));

	if(!p){
		p = malloc(sizeof(MSG_QUEUE));
	}
	
	p->ptr = malloc(size);
	p->size = size;
	p->msg_size = entry_size;

	SLL_INIT(&p->head);
	SLL_INIT(&p->free);


	pthread_mutex_init(&p->mutex,NULL);
	pthread_cond_init(&p->bRead,NULL);
	pthread_cond_init(&p->bWrite,NULL);

	data = p->ptr;
	for(i = 0;i < size / entry_size; i ++){
	
		sllPutAtTail(&p->free,(SL_NODE *)data);
		data = data + entry_size;
	}

	return p;
}

int msg_put(MSG_QUEUE * q,void * buffer,int size)
{
	SL_LIST * h;
	SL_LIST * f;
	MSG_NODE * node = NULL;
	int ret = -1;
	if(!q || !buffer) return -1;

	h = &q->head;
	f = &q->free;
	pthread_mutex_lock(&q->mutex);
	while(1){
		node =(MSG_NODE *)sllGet(f);
		
		if(node == NULL){
			ret = pthread_cond_wait(&q->bWrite,&q->mutex);
			if(ret > 0) break;
		}else{

			memcpy(node + MSG_NODE_SIZE,buffer,size);
			sllPutAtHead(h,&node->node);
			node->entry_size = size;
			pthread_cond_signal(&q->bRead);
			break;
		}


	}

	pthread_mutex_unlock(&q->mutex);
	return ret > 0 ? ret : size;
}

int msg_get(MSG_QUEUE * p,void * buffer,int size){
	
	SL_LIST * h = NULL;
	SL_LIST * f = NULL;
	//SL_NODE * n= NULL;
	MSG_NODE * node = NULL;
	int ret = -1;
	if(!p || !buffer ) return -1;

	f = &p->head;
	h = &p->head;

	pthread_mutex_lock(&p->mutex);
	while (1)
	{
		node =(MSG_NODE *)sllGet(h);
		if(node == NULL){
			ret = pthread_cond_wait(&p->bRead,&p->mutex);
			if(ret) break;
		}else{

			// check msg size
			if(node->entry_size < size){
				//Maybe something is error \n
				printf("Waring Msg size err ... \n");
				return -2;
			}

			memcpy(buffer,node+MSG_NODE_SIZE,node->entry_size);
			sllPutAtTail(f,&node->node);
			pthread_cond_signal(&p->bWrite);
			break;
		}
	}

	pthread_mutex_unlock(&p->mutex);

	return ret > 0 ? ret : node->entry_size; 
}


void msg_queue_destory(MSG_QUEUE * q){
	if(!q) return -1;

	pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->bRead);
	pthread_cond_destroy(&q->bWrite);

	if(q->ptr)
		free(q->ptr);
}


typedef struct msg
{
	int cmd;
	int data;
	/* data */
}MSG;


void send(void * arg){

	MSG_QUEUE * q = (MSG_QUEUE *)arg;
	MSG * pData;
	pData = malloc(sizeof(MSG));
	
	memset(pData,0,sizeof(MSG));

	while(1){
		
		pData->cmd += 1;
		pData->data += 1;

		msg_put(q,pData,sizeof(MSG));

		printf("Thread %d send cmd %d,data %d \n",pthread_self(),pData->cmd,pData->data);

		if(pData->data == 5){
			printf("send Thread exit ... \n");
			pthread_exit(0);
		}

		sleep(1);
	}

}


void recv(void * arg){

	MSG_QUEUE * q = (MSG_QUEUE *)arg;
	MSG pData;
	//pData = malloc(sizeof(MSG));
	
	while(1){
	
		msg_get(q,&pData,sizeof(MSG));

		printf("Thread %d recv cmd %d,data %d \n",pthread_self(),pData.cmd,pData.data);

		if(pData.data == 5){
			printf("recv Thread exit ... \n");
			pthread_exit(0);
		}

		usleep(500 * 1000); // 500 ms
	}

}

int main(int argc, char *argv[]) {
	printf("Hello World\n");
	
	pthread_t tid,tid1;
	MSG_QUEUE * q = NULL;

	q = msg_queue_init(10,sizeof(MSG));

	if(!q){
		printf("msg queue init failed \n");
		return -1;
	}

	pthread_create(&tid,NULL,recv,q);
	pthread_create(&tid1,NULL,send,q);

	pthread_join(tid1,NULL);
	pthread_join(tid,NULL);

	printf("main[%d] exit ...",getpid());
	return 0;
}