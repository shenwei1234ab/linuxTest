#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "wrap.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAXLINE 80
#define SERV_PORT 8001

int main(int argc, char *argv[])
{
struct sockaddr_in servaddr;
char buf[MAXLINE];
int sockfd, n;
sockfd = Socket(AF_INET, SOCK_STREAM, 0);
bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
inet_pton(AF_INET, "192.168.1.3", &servaddr.sin_addr);
printf(" sin_addr is  %u\n",servaddr.sin_addr.s_addr);
in_addr inAddr;
inAddr.s_addr = servaddr.sin_addr.s_addr;
//浣跨�inet_ntoa
printf("%s",inet_ntoa( inAddr));
//浣跨�inet_ntop
char ifBuf[50];
printf("%s",inet_ntop(AF_INET, &inAddr, ifBuf,sizeof(ifBuf)));
servaddr.sin_port = htons(SERV_PORT);
Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
while (fgets(buf, MAXLINE, stdin) != NULL) {
Write(sockfd, buf, strlen(buf));
n = Read(sockfd, buf, MAXLINE);
if (n == 0)
printf("the other side has been closed.\n");
else
Write(STDOUT_FILENO, buf, n);
}
//Close(sockfd);
return 0;
}
