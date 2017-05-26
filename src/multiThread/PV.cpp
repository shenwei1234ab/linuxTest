#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include "myipc_sem.h"
#include "myipc_shm.h"

#include <pthread.h>

int g_key = 0x3333;

void TestFunc(long loopnum)
{
	printf("loopnum:%ld\n", loopnum);
	
	int ncount = 0;
	int ret = 0;
	int shmhdl = 0;
	int *addr = NULL;
	
	int semid = 0;
	sem_open(g_key, &semid);


 	sem_p(semid); //p操作临界区开始 
	//
	ret = IPC_CreatShm(".", 0, &shmhdl);
		
	ret =IPC_MapShm(shmhdl, (void **)&addr);
	*((int *)addr) =  *((int *)addr)  + 1;
	ncount = *((int *)addr);
	printf("ncount:%d\n", ncount);
	//addr[0] = addr[0] +1;
	ret =IPC_UnMapShm(addr);
		//sleep(2);
		
	sem_v(semid);  //v操作临界区结束
	//
	printf("进程正常退出:%d\n", getpid());
}	



//posix 线程库的函数 线程库
void *thread_routine(void* arg)
{
	long num = (long)arg;
	printf("thread_routine start\n");
	TestFunc(num);
	pthread_exit(0);

} 

int main(void )
{
	int res;
    int procnum=10;
	long loopnum = 100;
	
	pthread_t tidArray[200];


	int  i=0,j = 0;

	printf("请输入要创建子进程的个数 : \n");
	scanf("%d", &procnum);

	printf("请输入让每个子进程测试多少次 :\n");
	scanf("%ld", &loopnum);
	//共享内存创建
	int		ret = 0;
	int 	shmhdl = 0;
	ret = IPC_CreatShm(".", sizeof(int), &shmhdl);
	if (ret != 0)
	{
		printf("func IPC_CreatShm() err:%d \n", ret);
		return ret;
	}
	
	
	//信号量的创建
 	int  	semid = 0;
	//ret = sem_creat(g_key, &semid);
	ret = sem_open(g_key, &semid);
	if (ret != 0)
	{
		printf("func sem_creat() err:%d,重新按照open打开信号量 \n", ret);
		if (ret == SEMERR_EEXIST)
		{
			ret = sem_open(g_key, &semid);
			if (ret != 0)
			{
				printf("按照打开的方式，重新获取sem失败:%d \n", ret);
				return ret;
			}
		}
		else
		{
			return ret;
		}
		
	}
	
	int  val = 0;
	ret = sem_getval(semid, &val);
	if (ret != 0 )
	{
		printf("func sem_getval() err:%d \n", ret);
		return ret;
	}
	printf("sem val:%d\n", val);
	getchar();
	
	for (i=0; i<procnum; i++)
	{
	
		//tmp.numId = i; //这说明 线程函数在使用了一个不断变化的内存空间。。。。
		pthread_create(&tidArray[i], NULL, thread_routine,(void *)loopnum);
	}
	
	for (i=0; i<procnum; i++)
	{
		pthread_join(tidArray[i], NULL); //等待线程结束。。。
	}


	printf("父进程退出 删除共享内存...\n");
	IPC_DelShm(shmhdl);
	return 0;
}




