
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>

//端口号1--65535  建议用1024之后的
#define       SER_PORT      10086
#define       SER_IP        "192.168.122.1"

#define       MAX_WAIT      10

void doServer(int newsfd)
{
    int i;
    for(i=0;i<10;i++)
    {
        //如果接收放掉线(关闭) 如同写一个没有读端的管道 收到SIGPIPE信号
        write(newsfd,"hello",5);
        sleep(1);
    }
    write(newsfd,"bye",3);
    shutdown(newsfd,SHUT_RDWR);
}

int main(void)
{
     int sockfd,ret;
     pid_t pid;

     signal(SIGCHLD,SIG_IGN);//不产生僵尸进程
      //0选默认协议
     sockfd=socket(AF_INET,SOCK_STREAM,0);
     if(sockfd==-1)
     {
        perror("create socket");
        return 1;
     }
     //描述主机的结构信息
     struct sockaddr_in  addr;
     addr.sin_family=AF_INET;//指定地址协议
     addr.sin_port  =htons(SER_PORT);//网络字节序(大端字节字节序)
     //addr.sin_addr.s_addr=inet_addr(SER_IP);
     addr.sin_addr.s_addr=INADDR_ANY;//指本机的所有IP(包括127.0.0.1 广播  多播)
     memset(addr.sin_zero,0,8);//填补字节 为匹配之前的结构struct sockaddr
     ret=bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));
     if(ret==-1)
     {
        perror("bind");
        shutdown(sockfd,SHUT_RDWR);
        return 2;
     }
     //监听:MAX_WAIT<半连接队列大小>
     ret=listen(sockfd,MAX_WAIT);
     if(ret==-1)
     {
        perror("bind");
        shutdown(sockfd,SHUT_RDWR);
        return 2;
     }
     printf("Server Start Success,Wait Connect...\n");
     //////////////////
     int newsfd;
     struct sockaddr_in   from;
     socklen_t   len=sizeof(from);
     while(1)
     {
        //连接没有到来则阻塞
        //newsfd=accept(sockfd,NULL,NULL);
        newsfd=accept(sockfd,(struct sockaddr *)&from,&len);
        if(newsfd==-1)  continue;//处理失败

        printf("sockfd:%d newsfd:%d from:%s<%d>\n",\
            sockfd,newsfd,inet_ntoa(from.sin_addr),ntohs(from.sin_port));


        //建立进程或线程服务
        pid=fork();
        if(pid==-1)
        {
           write(newsfd,"sorry,Can't server",sizeof("sorry,Can't server"));
           shutdown(newsfd,SHUT_RDWR);
        }
        else if(pid==0)//child
        {
           doServer(newsfd);
           exit(0);
        }
     }

     return 0;
}
