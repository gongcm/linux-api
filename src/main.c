#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "msg_queue.h"
/*
	sync interface  POSIX 可移植的标准接口
 */

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};


int interface_sem_init(int key,int init_data){
	int semid = -1;
	int val = init_data;
	
	semid = semget(key,1,IPC_CREAT | 0666);
	if(semid < 0){
		return -1;
	}

	if(semctl(semid,0,SETVAL,val) < 0){
		return -2;
	}

	return semid;
}

int interface_sem_p(int semid)
{
	struct sembuf sem;

	sem.sem_num = 0;
	sem.sem_op = -1;
	sem.sem_flg = SEM_UNDO;

	return semop(semid,&sem,1);
}

int interface_sem_v(int semid){
	struct sembuf sem;

	sem.sem_num = 0;
	sem.sem_op = 1;
	sem.sem_flg = SEM_UNDO;

	return semop(semid,&sem,1);
}

int interface_sem_release(int semid){

	return semctl(semid,0,IPC_RMID,0);
}


/*
	share memory POSIX
 */

int interface_shm_init(int key,int size){
	int shmid;
	shmid = shmget(key,size,IPC_CREAT | 0666);
	return shmid;
}

int interface_shm_write(int shmid,void * buffer,int size){
	struct shmid_ds buf;
	void * pshm = shmat(shmid,NULL,0);

	shmctl(shmid,IPC_STAT,&buf);
	if(buf.shm_segsz < size){
		printf("Warning memory overflow !!! ,shm_size %d ,size %d \n",buf.shm_segsz,size);
	}
	printf("write %d \n",(int)(*(int *)buffer));
	memcpy(pshm,buffer,size);

	shmdt(pshm);
	return size;
}


int interface_shm_read(int shmid,void * buffer,int size){
	struct shmid_ds buf;
	void * pshm = shmat(shmid,NULL,0);

	shmctl(shmid,IPC_STAT,&buf);
	if(buf.shm_segsz < size){
		printf("Warning memory overflow !!! ,shm_size %d ,size %d \n",buf.shm_segsz,size);
	}

	memcpy(buffer,pshm,size);

	shmdt(pshm);
	return size;
}


void * interface_shm_map(int shmid){
	return shmat(shmid,NULL,0);
}

int interface_shm_unmap(void * pshm){
	return shmdt(pshm);
}

int interface_shm_release(int shmid){
	return shmctl(shmid,IPC_RMID,NULL);
}


/*
	//在读锁时，该描述符必须是读打开；加写锁时，该描述符必须是写打开。
	创建一个文件来控制进程的同步等，
	该创建的文件仅仅记录文件锁的状态，不用来保存数据。
 */
#define path ".lock"

int rwlock_init() {
	int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	return fd;
}

int rwlock_release(int fd) {
	if (fd) {
		close(fd);
	}

	// 文件存在就会删除，就是减少文件的硬链接数，为 0 时 删除文件
	unlink(path);
	return 0;
}

int lock(int fd, int cmd, int lock) {

	struct flock fl;

	fl.l_len = 1;
	fl.l_start = 0;
	fl.l_type = lock;
	fl.l_whence = SEEK_SET;
	return fcntl(fd, cmd, & fl);
}


int lock_status(int fd) {
	int ret = -1;
	int status = 0;

	struct flock fl = {
		0
	};
	ret = lock(fd, F_GETLK,&fl);
	if (ret < 0) {
		printf("fcntl F_GETLK failed \n");
		return ret;
	}

	if (fl.l_len < 0) {
		status = -1;
		sprintf(stderr,"warning flock len %ld \n", fl.l_len);
		return status;
	}

	if (fl.l_type & F_RDLCK) {
		status = 1;
	}

	if (fl.l_type & F_WRLCK) {
		status += 2;
	}

	return status;
}


// F_SETLKW 设置互斥锁，W == wait，该锁获取不到时，会进行等待锁被其他进程释放掉。 可以用于进程间同步。
int read_lock(int fd) {
	return lock(fd, F_SETLK, F_RDLCK);
}

int read_lockW(int fd){
	return lock(fd,F_SETLKW,F_RDLCK);
}

int read_unlock(int fd) {
	return lock(fd, F_SETLK, F_UNLCK);
}

int write_lock(int fd) {
	return lock(fd, F_SETLK, F_WRLCK);
}

int write_lockW(int fd){
	return lock(fd,F_SETLKW,F_WRLCK);
}

int write_unlock(int fd) {
	return lock(fd, F_SETLK, F_UNLCK);
}

// test msg_queue
typedef struct Msg{
	int cmd; //命令
	int data; // 数据
}MsgNode;

void send_m(void *arg){
	MsgHeader * h = (MsgHeader *)arg;
	MsgNode node = {0};
	int ret = -1;
	while(1){
		
		node.cmd = 1;
		node.data += 1;
		ret = msg_put(h,&node,sizeof(MsgNode));
		if(ret < 0){
			continue;
		}

		printf("put ret %d ,cmd %d ,data %d \n",ret,node.cmd,node.data);
		usleep(500 * 1000);// 500ms

		if(node.data == 5){
			printf("send exit ... \n");
			sleep(1);
		 	break;
		}
	}
}


void recv_m(void *arg){
	MsgHeader * h = (MsgHeader *)arg;
	MsgNode node = {0};
	int ret = -1;
	while(1){
		
		//node.cmd = 1;
		//node.data += 1;
		ret = msg_get(h,&node,sizeof(MsgNode));
		printf("get ret %d cmd %d ,data %d \n",ret,node.cmd,node.data);
		usleep(500 * 1000);// 500ms

		if(node.data == 5){
			printf("recv exit ...\n");
			break;
		}
	}
}
int main(int argc, char * argv[]) {

	int ret = -1;
	int fd = -1;
	int exit_status = 0;
	int pid = -1;
	key_t key = ftok(".",0);

	key_t shm_key = ftok("..",0);

	int semid = interface_sem_init(key,0);
	int shmid = interface_shm_init(shm_key,1024);
	
	pid = fork();
	if (pid == 0) {

		/*
			子进程写数据
			然后读数据
		*/

		int count = 0;
		fd = rwlock_init();
		if (fd < 0) {
			printf("rwlock init failed \n");
			return fd;
		}
		printf("child fd %d \n",fd);
		while (1) {
			//printf("\n status : %d \n",lock_status(fd));

			if (write_lockW(fd) < 0) {
				printf("child,write lock failed,fd %d\n",fd);
			}

			count++;
			//siprintf(buffer,"%d\n",count);
			//memcpy(buffer,&count,sizeof(count));
			interface_shm_write(shmid,&count,sizeof(int));
			printf("child,count %d \n", count);
			if (count == 10) {
				write_unlock(fd);
				interface_sem_v(semid);
				sleep(1);
				exit(0);
				break;
			}

			if (write_unlock(fd) < 0) {
				printf("write unlock failed \n");
			}

			interface_sem_v(semid);
			printf("sem v \n");
			sleep(1);
		}
	}

	fd = rwlock_init();
	
	if (fd < 0) {
		printf("rwlock init failed \n");
		return fd;
	}
	printf("Parnet fd %d \n",fd);

	while (1) {

		printf("sem p \n");
		ret = interface_sem_p(semid);
		//printf("sem p over ,ret : %d \n",ret);
		if (write_lockW(fd) < 0) {
			printf("parent,write lock failed ,fd %d\n",fd);
		}

		//count++;
		//memcpy(&exit_status,buffer,sizeof(int));
		interface_shm_read(shmid,&exit_status,sizeof(int));
		printf("parent,count %d \n",exit_status);
		if (exit_status == 10) {
			write_unlock(fd);
			break;
		}

		if (write_unlock(fd) < 0) {
			printf("write unlock failed \n");
		}

		sleep(1);
	}

	waitpid(pid,NULL,0);
	printf(" exit ... \n");

	rwlock_release(fd);
	interface_sem_release(semid);
	interface_shm_release(shmid);
	// test msg queue

	pthread_t tid1,tid2;

	MsgHeader * h = msg_queue_init(5,sizeof(MsgNode));

	pthread_create(&tid2,NULL,recv_m,h);
	sleep(1);
	pthread_create(&tid1,NULL,send_m,h);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	
	msg_queue_release(h);
	usleep(100 * 1000);
	return 0;
}
