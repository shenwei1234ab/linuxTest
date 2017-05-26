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


//本机测试设置的大点，网络实际测试可能只有2M就阻塞了
#define MAXBUF 20*1000*1024
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

	char s[1024];
	char *pBuf = (char *)malloc(MAXBUF);
	int i = 0;
	while(1)
	{
			memset(s, 0, sizeof(1024));
			//strcpy(s, "hello world");
			read(STDIN_FILENO,s,sizeof(s));
			
			int ret = send(st, pBuf, MAXBUF, 0); //发送buf的数据
			i++;
			printf("i:%d send %d\n",i,ret);
			if(ret <= 0)
			{
				printf("send failed %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			/*memset(s, 0, sizeof(s));
			if(recv(st,pBuf,MAXBUF,0)>=0)
			{
				printf("recv %s \n",s);
			}
			else
			{
				printf("recv failed %s\n",strerror(errno));
				break;
			}*/
	}
	close(st);//关闭socket
	return EXIT_SUCCESS;

}
