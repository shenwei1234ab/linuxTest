#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include "wrap.h"
#include <ctype.h>
#include <unistd.h>

#define MAXLINE 80
#define SERV_PORT 8001
#define OPEN_MAX 1024

//myadd
int main(int argc, char *argv[])
{
	//test
	int i, j, maxi, listenfd, connfd, sockfd;
	int nready, efd, res;
	ssize_t n;
	char buf[MAXLINE], str[INET_ADDRSTRLEN];
	socklen_t clilen;
	int client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;
	struct epoll_event tep, ep[OPEN_MAX];
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	int on=1;
	//设置允许地址重用
	if((setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	Listen(listenfd, 20);
	for (i = 0; i < OPEN_MAX; i++)
	client[i] = -1;
	maxi = -1;

	efd = epoll_create(OPEN_MAX);
	if (efd == -1)
	{
		perr_exit("epoll_create");
	}
	tep.events = EPOLLIN; tep.data.fd = listenfd;
	res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
	if (res == -1)
	perr_exit("epoll_ctl");

	//jianting
	for ( ; ; )
	{
		nready = epoll_wait(efd, ep, OPEN_MAX, -1);
		if (nready == -1)
			perr_exit("epoll_wait");
		else
		{
			printf("nready %d \n",nready);
		}
		for (i = 0; i < nready; i++)
		{
			if (!(ep[i].events & EPOLLIN))
				continue;

			if (ep[i].data.fd == listenfd)
			{
				clilen = sizeof(cliaddr);
				connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
				printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
				/*for (j = 0; j < OPEN_MAX; j++)
				if (client[j] < 0)
				{
					client[j] = connfd;  save descriptor
					break;
				}
				if (j == OPEN_MAX)
				perr_exit("too many clients");

				if (j > maxi)
				maxi = j; */

				tep.events = EPOLLIN; tep.data.fd = connfd;
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if (res == -1)
				perr_exit("epoll_ctl");
			}
			else
			{
				sockfd = ep[i].data.fd;
				n = Read(sockfd, buf, MAXLINE);
				printf("n= %d\n",n);
				if(n == 0)
				{
					//close socket

					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
					if (res == -1)
					perr_exit("epoll_ctl");
					Close(sockfd);
					printf("client[%d] closed connection\n", j);
				}
				else
				{
					for (j = 0; j < n; j++)
					buf[j] = toupper(buf[j]);
					Writen(sockfd, buf, n);
				}
			}
		}
	}
	close(listenfd);
	close(efd);
	return 0;
}
