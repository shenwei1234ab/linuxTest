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
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//死锁情况
void *fun1(void *arg)
{
	pthread_mutex_lock(&mutex);
	printf("thread%x lock\n",pthread_self());
	for(int i=0;i<10;++i)
	{
		printf("thread%x i=%d\n",pthread_self(),i);
		sleep(1);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}


void *fun2(void *arg)
{
	pthread_t thr = *(pthread_t*)arg;
	sleep(1);
	printf("pthread_cancel id :%x\n",thr);
	pthread_cancel(thr);
	return NULL;
}

int main()
{
	pthread_t thread1,thread2,thread3;
	 pthread_create(&thread1,NULL,fun1,NULL);
	 pthread_create(&thread2,NULL,fun1,NULL);
	 printf("thread1_id is %x\n",thread1);
	 printf("thread2_id is %x\n",thread2);
	pthread_create(&thread3,NULL,fun2,&thread2);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
