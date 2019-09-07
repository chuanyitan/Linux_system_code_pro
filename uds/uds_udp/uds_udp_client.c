#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
 
#define SA struct sockaddr
#define PATHNAME "/tmp/unixudp"
 
void err_sys(const char *errmsg);
 
int main(void)
{
    int sockfd;
    struct sockaddr_un servaddr, localaddr;
    char buf[BUFSIZ], s[] = "hello china";
    ssize_t n;
 
    if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1)
        err_sys("socket");
 
    bzero(&localaddr, sizeof(localaddr));
    localaddr.sun_family = AF_LOCAL;
    strncpy(localaddr.sun_path, tmpnam(NULL), sizeof(localaddr.sun_path) - 1);
 
    if (bind(sockfd, (SA *)&localaddr, sizeof(localaddr)) == -1)
        err_sys("bind");
 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strncpy(servaddr.sun_path, PATHNAME, sizeof(servaddr.sun_path) - 1);
 
    for(;;){
        if (sendto(sockfd, s, strlen(s), 0, (SA *)&servaddr, sizeof(servaddr)) == -1)
            err_sys("sendto");
        if ((n = read(sockfd, buf, sizeof(buf))) == -1)
            err_sys("read");
        else 
            printf("%s\n", buf);
        usleep((rand() % 10) * 10000);
    }
 
    exit(0);
}
 
void err_sys(const char *errmsg)
{
    perror(errmsg);
    exit(1);
}
————————————————
版权声明：本文为CSDN博主「fulinux」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/fulinus/article/details/40659283