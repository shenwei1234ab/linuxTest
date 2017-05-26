#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
//#include "sckutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "commsocket.h"

void handle(int signum)
{
	int pid = 0;
	printf("recv signum:%d \n", signum);
	
	//避免僵尸进程
	while ((pid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("退出子进程pid%d \n", pid);
		fflush(stdout);
	} 
}



int main()
{	
	int 		ret = 0;
	void 		*handle = NULL;
	//void 		handle = NULL;
	int 		connfd;
	int 				i = 0, j = 0;
	
	signal(SIGCHLD, handle);
	signal(SIGPIPE, SIG_IGN); //防止 sokcet破裂
	
	int procnum=10;
	int loopnum = 100;
	
	pthread_t tidArray[1024*2];
	


	printf("请输入要创建子进程的个数 : \n");
	scanf("%d", &procnum);

	printf("请输入让每个子进程测试多少次 :\n");
	scanf("%d", &loopnum);
	
		
	 unsigned char *data = (unsigned char *)"aaaaaafffffffffffssssss";
	 int 		datalen = 10;
	 
	 unsigned char out[1024];
	 int outlen = 1024;
	//客户端环境初始化
	
	ret = sckCliet_init(&handle, 15, 5, 5, 10);

	for (i=0; i<procnum; i++)
	{
		int pid = fork();
		if (pid == 0)
		{
			ret = sckCliet_getconn(handle, "127.0.0.1", 8001, &connfd);
			for (j=0; j<loopnum; j++)
			{
				//客户端发送报文
				ret = sckClient_send(handle,  connfd, data, datalen);
				if (ret != 0)
				{
					if (ret == Sck_ErrTimeOut)
					{
						continue;
					}
					break;
				}
				//printf("ccccccc\n");
					//客户端端接受报文
				ret = sckClient_rev(handle, connfd, out, &outlen); //10
				if (ret != 0)
				{
					if (ret == Sck_ErrTimeOut)
					{
						break; 
					}
					break;
				}
				out[outlen] = '\0';
			
				printf("out: %s \n", out);
			}
			
			close(connfd);
			exit(0); //让子进程退出，不参与fork
		}
	}
	

	// 客户端环境释放 
	ret = sckClient_destroy(handle);
	
	//避免僵尸进程
	int mypid;
	while ((mypid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("退出子进程pid%d \n", mypid);
		fflush(stdout);
	} 

}

