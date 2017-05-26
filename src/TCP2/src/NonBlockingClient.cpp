/*
 ============================================================================
 Name        : sock.c
 Author      : zhujy
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
 #include <sys/types.h>
 #include <sys/stat.h>
  #include <fcntl.h>
#include <strings.h>
#include <unistd.h>
#include "pub.h"
using namespace std;
#define MAXBUF 1*1024*1024

void setnonblocking(int st)
{
	int opts = fcntl(st,F_GETFL);
	if(opts < 0)
	{
		return;
	}
	opts = opts|O_NONBLOCK;
	if(opts < 0)
	{
		return ;
	}
	fcntl(st,F_SETFL,opts);
}



void *recvsocket(void *arg)
{
	int st= *(int *)(arg);
	char s[1024];
	while(1)
	{
		memset(s,0,sizeof(s));
		unsigned int rc = recv(st,s,sizeof(s),0);
		if (rc == 0)
		{
			printf("Server socket closed\n");
			break;
		}
		else if(rc<0)
		{
			printf("recv failed %s\n", strerror(errno));
			break;
		}
		printf("server send %s\n",s);

	}
	return NULL;
}


int WriteFile(char pContext[],int length)
{
		char s[] = "abc.txt";
		int fd = open(s, O_RDWR | O_APPEND);//用读写追加方式打开文件
		if (fd == -1)
		{
			return -1;
			//printf("error is %s\n", strerror(errno));
		}
		else
		{
			//printf("success fd = %d\n", fd);
			//char buf[100];
			//memset(buf, 0, sizeof(buf));
			//strcpy(buf, pContextpContext);
			int i = write(fd, pContext, length);//这里要用strlen函数
			close(fd);
		}
		return 0;
}

void *sendsocket(void *arg)
{
		unsigned int data=0;
		int time=0;
		int st= *(int *)(arg);
		char s[1024];
		char *p=(char *)malloc(MAXBUF);
		while(1)
		{

			memset(s,0,sizeof(s));
			if(time==0)
			{
				read(STDIN_FILENO,s,sizeof(s));
				time = 1;
			}
			//fei阻塞
			unsigned int rc = write(st,p,MAXBUF);

			if(rc == -1 )
			{
				continue;
			}
			if(rc == 0)
			{
				continue;
			}
			if(rc >0)
			{
				data+=rc;
				printf("send data %d \n",rc);
				if(data >= MAXBUF)
				{
					printf("send data full \n");
					break;
				}
			}
		}
		return NULL;
}


int main(void)
{
	int st = socket(AF_INET, SOCK_STREAM, 0);//初始化socket，
	struct sockaddr_in addr;//定义一个IP地址的结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;//设置结构地址类型为TCP/IP地址
	addr.sin_port = htons(8080);//指定一个端口号：8080，htons:将short类型从host字节类型到net字节类型转化
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//将字符串类型的IP地址转化为int，赋给addr结构成员.

	//调用connect连接到结构addr指定的IP地址和端口号
	if (connect(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("connect failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	setnonblocking(st);
	char s[1024];
	pthread_t sendThread;
			pthread_t recvThread;
			pthread_create(&sendThread,NULL,sendsocket,&st);
			//pthread_create(&recvThread,NULL,recvsocket,&st);
			//pthread_detach(recvThread);
			pthread_join(sendThread,NULL);
	printf("Client exit");
	close(st);//关闭socket
	return EXIT_SUCCESS;

}
