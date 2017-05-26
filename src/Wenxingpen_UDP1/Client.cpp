#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "wrap.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXLINE 80
#define SERV_PORT 8000

#define MAXBUF 20*1000*1024
int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	int sockfd, n;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	socklen_t servaddr_len;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
	/*while (fgets(buf, MAXLINE, stdin) != NULL)
	{
		n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (n < 0 )
		{
			printf("sendto error");
		}
		else
		{
			printf("send ok ");
		}
		n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, 0);
		if (n == -1)
		printf("recvfrom error");
		write(STDOUT_FILENO, buf, n);
	}*/
	char s[1024];
	char *pBuf = (char *)malloc(MAXBUF);
	while (1)
	{
		memset(s, 0, sizeof(1024));
		//strcpy(s, "hello world");
		read(STDIN_FILENO, s, sizeof(s));
		//数据报过大sendto会直接返回错误
		int ret = sendto(sockfd, pBuf, MAXBUF, 0,(struct sockaddr *)&servaddr, sizeof(servaddr)); 
		//发送buf的数据
		if (ret <= 0)
		{
			printf("send failed %s\n", strerror(errno));
			return -1;
		}
	}
	close(sockfd);
	return 0;

}
