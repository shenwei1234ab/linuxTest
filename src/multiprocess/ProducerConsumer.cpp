#include "semtest.h"
#include <pthread.h>
#define CUSTOM_COUNT 2
#define PRODUCT_COUNT 4

int max_product_num = 5;
int g_count = 0;
//使用信号量生产者和消费者
//互斥信号量
CSemaphore mutex(0x1112);
//同步信号量两个信号量分布在不同的进程中实现同步
CSemaphore empty(0x1113);	//生产者能在缓冲区放多少max_product_num
CSemaphore full(0x1114);	//消费者什么时候能从缓冲区拿0

void *product(void *arg)
{
	long num = (long)arg;
	while (1)
	{
		sleep(5);
		empty.Sem_p();			
		mutex.Sem_p();
		g_count++;
		printf("生产产品is%d\n", g_count);
		mutex.Sem_v();
		full.Sem_v();
	}
	pthread_exit(NULL);
}
//不加条件变量忙等
void *consume(void *arg)
{
	long num = (long)arg;
	while (1)
	{
		sleep(5);
		full.Sem_p();
		mutex.Sem_p();
		g_count--;
		printf("消费产品is%d\n", g_count);
		mutex.Sem_v();
		empty.Sem_v();
	}
	pthread_exit(NULL);
}

int forkTest()
{
	int status, i;
	for (i = 0; i < 10; i++)
	{
		status = fork();
		if (status == 0 || status == -1)
		{
			break;
		}//每次循环时，如果发现是子进程就直接从创建子进程的循环中跳出来，不让你进入环，这样就保证了每次只有父进程来做循环创建子进程的工作
	}
	if (status == -1)
	{
		return -1;
	}
	else if (status == 0) //每个子进程都会执行的代码
	{
		//sub process
	}
	else
	{
		//parent process
	}
}

int main()
{
	if (mutex.Init(1) < 0 || empty.Init(max_product_num) < 0 || full.Init(0) < 0)
	{
		printf("seminit failed");
	}
	else
	{
		printf("start");
	}
	//empty.Init(max_product_num);
	//full.Init(0);
	pthread_t tidArray[CUSTOM_COUNT + PRODUCT_COUNT];
	for (int i = 0; i<CUSTOM_COUNT; ++i)
	{
		pthread_create(&tidArray[i], NULL, consume, &i);
	}
	sleep(1);

	for (int i = 0; i<PRODUCT_COUNT; ++i)
	{
		pthread_create(&tidArray[i + CUSTOM_COUNT], NULL, product, &i);
	}


	for (int i = 0; i<CUSTOM_COUNT + PRODUCT_COUNT; ++i)
	{
		pthread_join(tidArray[i], NULL);
	}
	printf("main exit\n");
	return 0;
	exit(0);
}



