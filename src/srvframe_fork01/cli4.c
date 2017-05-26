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
	
	//���⽩ʬ����
	while ((pid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("�˳��ӽ���pid%d \n", pid);
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
	signal(SIGPIPE, SIG_IGN); //��ֹ sokcet����
	
	int procnum=10;
	int loopnum = 100;
	
	pthread_t tidArray[1024*2];
	


	printf("������Ҫ�����ӽ��̵ĸ��� : \n");
	scanf("%d", &procnum);

	printf("��������ÿ���ӽ��̲��Զ��ٴ� :\n");
	scanf("%d", &loopnum);
	
		
	 unsigned char *data = (unsigned char *)"aaaaaafffffffffffssssss";
	 int 		datalen = 10;
	 
	 unsigned char out[1024];
	 int outlen = 1024;
	//�ͻ��˻�����ʼ��
	
	ret = sckCliet_init(&handle, 15, 5, 5, 10);

	for (i=0; i<procnum; i++)
	{
		int pid = fork();
		if (pid == 0)
		{
			ret = sckCliet_getconn(handle, "127.0.0.1", 8001, &connfd);
			for (j=0; j<loopnum; j++)
			{
				//�ͻ��˷��ͱ���
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
					//�ͻ��˶˽��ܱ���
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
			exit(0); //���ӽ����˳���������fork
		}
	}
	

	// �ͻ��˻����ͷ� 
	ret = sckClient_destroy(handle);
	
	//���⽩ʬ����
	int mypid;
	while ((mypid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("�˳��ӽ���pid%d \n", mypid);
		fflush(stdout);
	} 

}

