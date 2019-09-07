//实现双向通信 收到数据 确认  收到确认在发下一个数据
//bugs:收到包没有及时回复 可能收到重复的包
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static sigjmp_buf env;

void timeOut(int signo)
{
    if(signo==SIGALRM)
    {
       siglongjmp(env,110);
    }
}

int main(void)
{
     int sockfd,ret;

     sockfd=socket(AF_INET,SOCK_DGRAM,0);
     if(sockfd==-1)
     {
        perror("create socket");
        return 1;
     }

     //初始化目标主机信息
     struct sockaddr_in  dest;
     dest.sin_family=AF_INET;
     dest.sin_port  =htons(10086);//UDP TCP端口独立不冲突
     dest.sin_addr.s_addr=inet_addr("192.168.2.122");
     memset(dest.sin_zero,0,8);

     int count=0;
     char buf[1024+4];

     signal(SIGALRM,timeOut);
     while(1)
     {
again:
        *(int *)buf=count;//包的序号
        snprintf(buf+4,1024,"udp data.");//包数据
        ret=sendto(sockfd,buf,4+strlen(buf+4),0,(struct sockaddr *)&dest,sizeof(dest));
        printf("send to %dth packet ret:%d\n",count,ret);

        //等待确认  超时重发
        ret=sigsetjmp(env,1);//设置跳转点
        if(ret==0)
        {
           alarm(3);
           ret=recvfrom(sockfd,buf,1024,0,NULL,NULL);
           alarm(0);//停止定时器
           if(ret==0||ret==-1)  break;
        }
        else
        {
            goto  again;
        }

        buf[ret]='\0';
        //if(*(int *)buf>count)
        {
            //下一个数据包
            count=*(int *)buf;
        }
     }


     shutdown(sockfd,SHUT_RDWR);
     return 0;
}
