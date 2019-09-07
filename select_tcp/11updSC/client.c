
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define        SER_PORT       10086
#define        SER_IP         "192.168.2.75"
#define        SIZE           1024


int main(void)
{
    int sockfd,ret;
    struct sockaddr_in ser_addr;
    char   buf[SIZE+1];

    //create socket
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==-1)
    {
        perror("create socket");
        return 1;
    }
    //初始化连接的服务的信息
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port  =htons(SER_PORT);
    ser_addr.sin_addr.s_addr=inet_addr(SER_IP);
    memset(ser_addr.sin_zero,0,8);
    //发送连接请求
    sendto(sockfd,"CONNECT",7,0,(struct sockaddr *)&ser_addr,sizeof(ser_addr));

    //接收数据
    while(1)
    {
       ret=recvfrom(sockfd,buf,SIZE,0,NULL,NULL);
       if(ret==0||ret==-1)  break;

       buf[ret]='\0';
       printf("%s\n",buf);
    }

    shutdown(sockfd,SHUT_RDWR);
    return 0;
}
