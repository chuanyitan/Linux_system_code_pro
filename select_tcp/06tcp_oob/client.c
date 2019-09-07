//TCP OOB数据接收需要开启信号IO
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>
#include <fcntl.h>


//端口号1--65535  建议用1024之后的
#define       SER_PORT      10086
#define       SER_IP        "192.168.2.75"

#define       SIZE          4096

static int sockfd;

void sigFunc(int signo)
{
    char buf[SIZE];
    int ret;
    printf("a sig recv:%d\n",signo);
    if(signo==SIGURG)
    {
        ret=recv(sockfd,buf,SIZE,MSG_OOB);
        if(ret==0||ret==-1)  return;

        buf[ret]='\0';
        printf("ret:%d紧急数据<>:%s\n",ret,buf);
    }
}

int main(void)
{
     int ret;


     system("clear");
     //0选默认协议
     sockfd=socket(AF_INET,SOCK_STREAM,0);
     if(sockfd==-1)
     {
        perror("create socket");
        return 1;
     }

     //开启套接字信号IO《异步IO》
     ret=fcntl(sockfd,F_SETOWN,getpid());
     if(ret==-1)
     {
         printf("当前进程对sockfd开启信号IO失败.\n");
         return 22;
     }
     signal(SIGURG,sigFunc);

     //初始化服务器主机的结构信息
     struct sockaddr_in  addr;
     addr.sin_family=AF_INET;//指定地址协议
     addr.sin_port  =htons(SER_PORT);//网络字节序(大端字节字节序)
     addr.sin_addr.s_addr=inet_addr(SER_IP);
     memset(addr.sin_zero,0,8);//填补字节 为匹配之前的结构struct sockaddr

     int count=0;
     while(1)
     {
         ret=connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
         if(ret==0)    break;//success

         switch(count){
            case 0:printf("\033[1;1H\033[326mConnect To Server.\033[0m\n");break;
            case 1:printf("\033[1;1H\033[326mConnect To Server..\033[0m\n");break;
            case 2:printf("\033[1;1H\033[326mConnect To Server...\033[0m\n");break;
         }
         count=(count+1)%3;
         usleep(800000);
     }

     //recv data
     char buf[SIZE+1];
     while(1)
     {
        //如果对方掉线 read返回0
        ret=recv(sockfd,buf,SIZE,0);
        if(ret==0 || ret==-1)   break;//0:dist connect
        buf[ret]='\0';
        printf("server say>%s\n",buf);
     }

     shutdown(sockfd,SHUT_RDWR);//close
     return 0;
}
