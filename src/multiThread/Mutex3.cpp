//============================================================================
// Name        : thread2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int nNum = 5;
int nLoop = 4;

int g_num=0;
struct ThreadInfo
{
	int threadid;
};

void *routine1(void *arg)
{
	printf("thread1\n");
	pthread_mutex_lock(&mutex);
	printf("thread1 lock\n");
	sleep(10);
	printf("thread 1 unlock\n");
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}




void *routine2(void *arg)
{
	printf("thread2\n");
	pthread_mutex_lock(&mutex);
	printf("thread 2 lock\n");
	sleep(10);
	printf("thread 2 unlock\n");
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

int main()
{
	pthread_t tidArray[200];
	ThreadInfo threadinfoArray[nNum];

	pthread_create(&tidArray[0],NULL,routine1,NULL);
	pthread_create(&tidArray[1],NULL,routine2,NULL);

	pthread_join(tidArray[0],NULL);
	pthread_join(tidArray[1],NULL);
	printf("main exit\n");
	return 0;
}

