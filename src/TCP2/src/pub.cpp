#include "pub.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
 #include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 1024


//set socket to noblocking

void setnonblocking(SOCKET st)
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


SOCKET init_socket()
{
	return 0;
}

SOCKET create_send_socket(const char *phost,int port,const char *pfilename)
{
/*	if(init_socket() == -1)
	{
		return 0;
	}*/
	SOCKET st = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;//定义一个IP地址的结构
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;//设置结构地址类型为TCP/IP地址
		addr.sin_port = htons(port);//指定一个端口号：8080，htons:将short类型从host字节类型到net字节类型转化
		addr.sin_addr.s_addr = inet_addr(phost);//将字符串类型的IP地址转化为int，赋给addr结构成员.
		//调用connect连接到结构addr指定的IP地址和端口号
		if (connect(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		{
			printf("connect failed %s\n", strerror(errno));
			return -1;
		}
		return st;
}

SOCKET create_recv_socket(int port)
{
/*	if(init_socket() == -1)
	{
		return 0;
	}*/
	SOCKET st = socket(AF_INET,SOCK_STREAM,0);
	if(st == 0)
	{
		return 0;
	}
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(st,(struct sockaddr *)&addr,sizeof(addr)) ==-1)
	{
		printf("bind failed %s\n",strerror(errno));
		return 0;
	}
	if(listen(st,20) == -1)
	{
		printf("listen failed %s\n",strerror(errno));
		return -1;
	}
	return st;
}



SOCKET socket_accept(SOCKET listen_st)
{
	struct sockaddr_in client_addr;
	unsigned int len = 1;
	len = sizeof(client_addr);
	//因为listen_st 为非阻塞所以accpet 立刻返回
	while(1)
	{
		memset(&client_addr,0,sizeof(client_addr));
		SOCKET client_st = accept(listen_st,(struct sockaddr *)&client_addr,&len);
		printf("accept return\n");
		if(client_st == -1)
		{
			//不是出错了，只是没有数据
			if(errno == EWOULDBLOCK)
			{
				sleep(1);
				continue;
			}
		}
		else
		{

			return client_st;
		}
	}
}


//client :
//send filecontext to server
int send_work(const char *phost,int port,const char *pfilename )
{
	SOCKET st_send =create_send_socket(phost,port,pfilename);
	//SOCKET st_recv =create_recv_socket(port+1);
	if(st_send == -1)
	{
		return -1;
	}
	printf("connect success\n");
	FILE *pFd = fopen(pfilename,"rb");
	if(pFd == NULL)
	{
		printf("open %s file failed %s\n",pfilename,strerror(errno));
		return 0;
	}
	char *pbuf =(char *) malloc(BUFSIZE);
	memset(pbuf,0,BUFSIZE);
	strcpy(pbuf,pfilename);
	//getfilename(pfilename,pbuf);

	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr=inet_addr(phost);
	printf("client send file:%s to %s:%d\n",pfilename,phost,port);
	size_t rc = send(st_send,pbuf,strlen(pbuf),0);
	if(rc == -1)
	{
		printf("send to server failed :%s\n ",strerror(errno));
		return -1;
	}
	else
	{
		printf("send to server success\n");
		struct sockaddr_in client_addr;
		memset(&client_addr,0,sizeof(client_addr));
		unsigned int len = sizeof(client_addr);
		memset(pbuf,0,BUFSIZE);
		if(recv(st_send, pbuf,  BUFSIZE, 0) <0)
		{
			printf("recv %s from server failed : %s\n",pbuf,strerror(errno));
		}
		else
		{
			//printf("receive message from %s\n",inet_ntoa(client_addr.sin_addr));
			if(strncmp(pbuf,"OK",2) == 0)
			{
				while(1)
				{
					memset(pbuf,0,BUFSIZE);
					//rc = fread(pbuf,1,BUFSIZE,pFd);
					read(STDIN_FILENO,pbuf,BUFSIZE);
					if(strncmp("end",pbuf,3) == 0)
					{
						break;
					}
					else
					{
						rc = send(st_send,pbuf,strlen(pbuf),0);
						if(rc < 0)
						{
							printf("send failed");
							break;
						}
					}
				}

			}
			memset(pbuf,0,BUFSIZE);
			rc = send(st_send,pbuf,128,0);
		}
	}
	fclose(pFd);
	close(st_send);
	//close(st_recv);
	return 1;
}

//server
int recv_work(int port)
{
	//创建listen socket
	SOCKET st_listen =create_recv_socket(port);
	//设置listen socket为非阻塞
	setnonblocking(st_listen);
	if(st_listen == 0)
	{
		return 0;
	}
	else
	{
		printf("server is listening.......\n");
	}
	SOCKET st_client =socket_accept(st_listen);
	//设置client_socket 为非阻塞
	setnonblocking(st_client);
	unsigned int len=1;
/*	struct sockaddr_in client_addr;
	len = sizeof(client_addr);
	memset(&client_addr,0,sizeof(client_addr));*/
	char *pbuf =(char *) malloc(BUFSIZE);
	memset(pbuf,0,BUFSIZE);
	int rc = recv(st_client, pbuf,  BUFSIZE, 0);
	if(rc < 0 )
	{
		printf("recv failed:\n",strerror(errno));
	}
	else if (rc == 0)
	{
		printf("client  conneted refused \n",strerror(errno));
	}
	else
	{
		printf("recv success:\n");
		//printf("receive message from %s\n",inet_ntoa(client_addr.sin_addr));
		printf("filename is %s\n",pbuf);
	   FILE *pFile =fopen(pbuf,"wb");
	   if(pFile == NULL)
	   {
		   printf("open file failed\n");
	   }
	   else
	   {
		   //client_addr.sin_port = htons(port+1);
		   memset(pbuf,0,BUFSIZE);
		   strcpy(pbuf,"OK");
		   //printf("send ok to %d len is %d\n",client_addr.sin_port,strlen(pbuf));

		   if( send(st_client,pbuf,strlen(pbuf),0)<0)
		   {
			   printf("server send ok failed\n");
		   }
		   else
		   {
			   while(1)
			   {
				   	   memset(pbuf,0,BUFSIZE);
				   	   int rc = recv(st_client,pbuf,BUFSIZE,0);
				   	   char tmp[128];
				   	   memset(tmp,0,sizeof(tmp));
				   	   if(memcmp(pbuf,tmp,sizeof(tmp)) == 0)
				   	   {
				   		   break;
				   	   }
				   	   else
				   	   {
				   		   	   fwrite(pbuf,1, rc,pFile);
				   	   }
			   }

		   }
	   }
	   if (pFile)
	   {
		   fclose(pFile);
	   }
	}
	free(pbuf);



}




