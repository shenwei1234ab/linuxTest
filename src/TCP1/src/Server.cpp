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
#include <signal.h>



#define MAXBUF 8*1024

bool Init()
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;//设定接受到指定信号后的动作为忽略
	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) == -1 ||   //初始化信号集为空
		sigaction(SIGPIPE, &sa, 0) == -1) 
	{   //屏蔽SIGPIPE信号
		perror("failed to ignore SIGPIPE; sigaction");
		exit(EXIT_FAILURE);
	}
}

//捕获sigpipe信号

int main(int arg, char *args[])
{
	Init();
	int listenST = socket(AF_INET, SOCK_STREAM, 0);//初始化socket
	int on = 1;
	if (setsockopt(listenST, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	int nRecvBuf = 60*1024;
	int tmp = 0;
	socklen_t optlen = sizeof(int);
	//设置接收缓冲区大小
	if (setsockopt(listenST, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	getsockopt(listenST, SOL_SOCKET, SO_RCVBUF, (int *)&tmp, &optlen);
	printf("recv_tmp=%d,optlen=%d\n", tmp, (int)optlen);

	struct sockaddr_in addr;//定义一个IP地址结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;//将addr结构的属性定位为TCP/IP地址
	addr.sin_port = htons(8080);//将本地字节顺序转化为网络字节顺序。
	addr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY代表这个server上所有的地址

	//将IP与server程序绑定
	if (bind(listenST, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("bind failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	//server端开始listen，
	if (listen(listenST, 20) == -1)
	{
		printf("listen failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	char s[1024];
	int client_st = 0;//client端socket
	//socklen_t len = 0;
	struct sockaddr_in client_addr;//表示client端的IP地址
	//void *p = &client_addr;
	int i;
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t len = sizeof(client_addr);
		//accept会阻塞，直到有客户端连接过来，accept返回client的socket描述符
		for(int i=0;i<4;++i)
		{
			printf("accept blocking \n");
			client_st = accept(listenST, (struct sockaddr *)&client_addr , &len);
			//testServer一直不接挤压缓冲区
			//sleep(30);
			//printf("sleep finish");
			if (client_st == -1)
			{
				printf("accept failed %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			printf("client_st is  %d\n ",client_st);
			while (1)
			{
				memset(s, 0, sizeof(s));
				char *pBuf = (char *)malloc(MAXBUF);
				int rc = recv(client_st, pBuf, MAXBUF, 0);//recv是阻塞调用，
				if (rc > 0)//接收来自client的消息
				{
					printf("recv %d\n", rc);
				}
				else
				{
					printf("revc error ret is %d msg is %s\n",rc, strerror(errno));
					//客户端已经关闭了。
					//这时在调用send会产生sigpipe信号导致服务器进程退出,解决捕获这个信号
					rc = send(listenST, s, strlen(s), 0);
					if (rc < 0)
					{
						printf("send error ret is %d \n", rc);
						printf("errno.%02d is: %s/n", errno, strerror(errno));
					}
					break;
				}
			}
			//close(client_st);
			continue;
		}

		while(1)
		{
			memset(s, 0, sizeof(s));
			int rc = recv(client_st, s, sizeof(s), 0);//recv是阻塞调用，
			if (rc > 0)//接收来自client的消息
			{
				printf("revc is %s\n", s);
			}
			else
			{
				if (rc == 0)
				{
					printf("client socket closed\n");
				}
				else
				{
					printf("recv failed %s\n", strerror(errno));
				}
				break;
			}
			memset(s, 0, sizeof(1024));
			//strcpy(s, "hello world");
			read(STDIN_FILENO,s,sizeof(s));
			if (send(listenST, s, strlen(s), 0) == -1)//发送buf的数据
			{
				printf("send failed %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
		}
		close(client_st);//关闭client端socket
	close(listenST);//关闭server端listen的socket
	return EXIT_SUCCESS;
}
