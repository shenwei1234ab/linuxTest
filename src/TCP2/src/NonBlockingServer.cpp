/*
 ============================================================================
 Name        : sock.c
 Author      : zhujy
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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
#include <unistd.h>
#include <fcntl.h>
#define SOCKET_T int
#define MAXBUF 1*1024*1024
struct ps
{
	SOCKET_T st;
	pthread_t *thr;
};


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

	struct ps *p = (struct ps *)arg;
	int st= p->st;
	char s[1024];
	char *pBuf=(char *)malloc(MAXBUF);
	while(1)
	{
		memset(s,0,sizeof(s));
		//int rc = recv(st,pBuf,MAXBUF,0);
		 int rc = read(st,pBuf,MAXBUF);
		if (rc == 0)
		{
			printf("client socket closed\n");
			break;
		}
		if(rc < 0)
		{
				continue;
		}
		if(rc > 0)
		{
			printf("Server  receive  %d data  \n",rc);
		}

	}
	pthread_cancel(*(p->thr));
	return NULL;
}

void *sendsocket(void *arg)
{
	printf("sendsocket thread\n");
		int st= *(int *)(arg);
		char s[1024];
		while(1)
		{
			memset(s,0,sizeof(s));
			read(STDIN_FILENO,s,sizeof(s));
			unsigned int rc = send(st,s,sizeof(s),0);
			if(rc <=0 )
			{
				break;
			}
			//printf("%s\n",s);
		}
		return NULL;
}



int main(int arg, char *args[])
{
	int st = socket(AF_INET, SOCK_STREAM, 0);//初始化socket
	int on = 1;
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}


	struct sockaddr_in addr;//定义一个IP地址结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;//将addr结构的属性定位为TCP/IP地址
	addr.sin_port = htons(8080);//将本地字节顺序转化为网络字节顺序。
	addr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY代表这个server上所有的地址

	//将IP与server程序绑定
	if (bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("bind failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	//server端开始listen，
	if (listen(st, 20) == -1)
	{
		printf("listen failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	char s[1024];
	SOCKET_T client_st = 0;//client端socket
	//socklen_t len = 0;
	struct sockaddr_in client_addr;//表示client端的IP地址
	//void *p = &client_addr;
	while(1)
	{
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t len = sizeof(client_addr);
		//accept会阻塞，直到有客户端连接过来，accept返回client的socket描述符
		client_st = accept(st, (struct sockaddr *)&client_addr , &len);
		if (client_st == -1)
		{
			printf("accept failed %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		//set nonblocking
		setnonblocking(client_st);
		pthread_t sendThread;
		pthread_t recvThread;

		ps ps1;
		ps1.st =client_st;
		ps1.thr = &sendThread;
		pthread_create(&recvThread,NULL,recvsocket,&ps1);
		//pthread_create(&sendThread,NULL,sendsocket,&client_st);
		pthread_join(recvThread,NULL);
		close(client_st);//关闭client端socket
		close(st);//关闭server端listen的socket
		return EXIT_SUCCESS;
	}
}
