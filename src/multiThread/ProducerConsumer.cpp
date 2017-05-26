
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

/*
 *
 * 多线程消费者生产者模型
 *
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	//控制互斥
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;	//控制同步
#define CUSTOM_COUNT 2
#define PRODUCT_COUNT 4

int nNum = 5;
int nLoop = 4;
int g_count = 0;

//消费者
void *consume(void *arg)
{
	long num = (long )arg;
	while(1)
	{
		
		pthread_mutex_lock(&mutex);
		while(g_count == 0)		//while 醒来以后需要重新判断 条件g_Count是否满足，如果不满足，再次wait
		{
			//当消费者
			printf("consumer% dsleep\n",num);
			pthread_cond_wait(&g_cond, &mutex); //api做了三件事情 //pthread_cond_wait假醒(可中断)
			////暂时放弃锁进入睡眠，让其他线程有条件获得锁
			//printf("consumer% dawake\n",num);
		}
		g_count--;
		printf("当前产品数量 is %d\n", g_count);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}

//不加条件变量
void *productError(void *arg)
{
	long num = (long)arg;
	while (1)
	{
		sleep(10);
		pthread_mutex_lock(&mutex);
		g_count++;
		printf("生产产品is%d\n", g_count);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}
//不加条件变量忙等
void *consumeError(void *arg)
{
	long num = (long )arg;
	while(1)
	{
		sleep(4);
		pthread_mutex_lock(&mutex);
		if (g_count != 0) 
		{
			//
			g_count--;
			printf("消费产品is%d\n", g_count);
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}




//生产者线程控制产品数量
void *product2(void *arg)
{
	long num = (long)arg;
	while (1)
	{
		pthread_mutex_lock(&mutex);
		//控制数量
		if (g_count >= 10)
		{
			printf("当前产品数量太多sleep  is %d\n", g_count);
			pthread_mutex_unlock(&mutex);
			sleep(20);
			continue;
		}
		g_count++;
		printf("当前产品数量 is %d\n", g_count);
		pthread_cond_signal(&g_cond);		//通知生产者重新获得锁
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}

//生产者线程不控制产品数量
void *product(void *arg)
{
	long num = (long)arg;
	while (1)
	{
		pthread_mutex_lock(&mutex);
		g_count++;
		printf("当前产品数量 is %d\n", g_count);
		pthread_cond_signal(&g_cond);		//通知生产者重新获得锁
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}


int main()
{
	pthread_t tidArray[CUSTOM_COUNT+PRODUCT_COUNT];
 	for(int i=0;i<CUSTOM_COUNT;++i)
	{
		pthread_create(&tidArray[i],NULL, consumeError,&i);
	}
 	sleep(1);

	for(int i=0;i<PRODUCT_COUNT;++i)
	{
		pthread_create(&tidArray[i+CUSTOM_COUNT],NULL, productError,&i);
	}


	for(int i=0;i<CUSTOM_COUNT+PRODUCT_COUNT;++i)
	{
		pthread_join(tidArray[i],NULL);
	}
	printf("main exit\n");
	return 0;
}

