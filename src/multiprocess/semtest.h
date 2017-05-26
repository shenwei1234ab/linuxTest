//使用 SYSTEM V信号量函数完成进程间互斥和同步
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define DELAY_TIME  3


union semun 
{
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
							 (Linux-specific) */
};
//没有就创建
int sem_create(key_t key)
{
	int semid = 0;
	semid = semget(0x1111,1,0666|IPC_CREAT|IPC_EXCL);
	if (semid == -1)
	{
		perror("semget error");
		if (errno == EEXIST)
		{
			printf("sem is alread exist");
			return 0;
		}
	}
	return semid;
}
//有就打开
int sem_open(key_t key)
{
	int semid = 0;
	//semid = semget(0x1111, 1, 0666 );
	semid = semget(0x1111, 1, IPC_CREAT | 0664);
	if (semid == -1)
	{
		perror("semget error");
		if (errno == EEXIST)
		{
			printf("sem is alread exist");
			return 0;
		}
	}
	return semid;
}

//设置信号量值（1,互斥）
int sem_setval(int semid,int val)
{
	int ret = 0;
	union semun su;
	su.val = val;
	ret = semctl(semid, 0, SETVAL, su);
	return ret;
}

int sem_getval(int semid)
{
	int ret = 0;
	union semun su;
	int val = su.val ;
	ret = semctl(semid, 0, SETVAL, su);
	printf("val :%d\n", val);
	return ret;
}

//P操作
int sem_p(int semid)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(semid, &sem_b, 1) == -1)
	{
		printf("p operation failed");
		return -1;
	}
	return 0;
}
//V操作
int sem_v(int semid)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(semid, &sem_b, 1) == -1)
	{
		printf("v operation failed");
		return -1;
	}
	return 0;
}



class CSemaphore
{
public:
	CSemaphore(key_t key) :m_key(key), m_sem_id(-1)
	{

	}
	int Init(int val)
	{
		//创建型号量
		m_sem_id = semget(m_key, 1, IPC_CREAT | 0664);
		if (m_sem_id == -1)
		{
			perror("semget error");
			if (errno == EEXIST)
			{
				printf("sem is alread exist");
				return -1;
			}
		}
		//设置信号量的值
		int ret = 0;
		union semun su;
		su.val = val;
		ret = semctl(m_sem_id, 0, SETVAL, su);
		return ret;
	}
public:
	int Sem_p()
	{
		if (m_sem_id == -1)
		{
			printf("sem_id is -1");
			return -1;
		}
		struct sembuf sem_b;
		sem_b.sem_num = 0;
		sem_b.sem_op = -1;
		sem_b.sem_flg = SEM_UNDO;
		if (semop(m_sem_id, &sem_b, 1) == -1)
		{
			printf("p operation failed");
			return -1;
		}
		return 0;
	}
	int Sem_v()
	{
		if (m_sem_id == -1)
		{
			printf("sem_id is -1");
			return -1;
		}
		struct sembuf sem_b;
		sem_b.sem_num = 0;
		sem_b.sem_op = 1;
		sem_b.sem_flg = SEM_UNDO;
		if (semop(m_sem_id, &sem_b, 1) == -1)
		{
			printf("v operation failed");
			return -1;
		}
		return 0;
	}
private:
	int m_sem_id;
	key_t m_key;

};

//信号量实现互斥
int mutexTest()
{
	//没有就创建一个key的信号量 查看ipcs命令
	//int semid = sem_create(0x1111);
	int semid = sem_open(0x1111);
	//设置为1相等于加锁了
	sem_setval(semid, 1);
	sem_getval(semid);

	int pid = 0;
	pid = fork();
	sem_p(semid);
	//临界区
	int i = 0;
	printf("i:%d pid:%d\n", ++i, getpid());
	sleep(3);
	printf("i:%d pid:%d\n", ++i, getpid());
	printf("dddddddddd\n");
	sem_v(semid);
	printf("dddddddddd");
	return 1;
}






