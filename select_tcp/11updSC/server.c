
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

pthread_mutex_t  mm=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   ok=PTHREAD_COND_INITIALIZER;
static int flag=1;

struct client{
     int  sfd;
     struct sockaddr_in  dest;
};

void *doServer(void *arg)
{
    pthread_mutex_lock(&mm);
    struct client  cc=*(struct client *)arg;
    flag=0;
    pthread_mutex_unlock(&mm);
    pthread_cond_signal(&ok);

    int idx;

    pthread_detach(pthread_self());

    for(idx=0;idx<20;idx++)
    {
        sendto(cc.sfd,"abcde",5,0,(struct sockaddr *)&cc.dest,sizeof(cc.dest));
        sleep(1);
    }
    //用于结束
    sendto(cc.sfd,NULL,0,0,(struct sockaddr *)&cc.dest,sizeof(cc.dest));
    pthread_exit((void *)0);
}

int main(void)
{
    int sockfd,ret;
    struct sockaddr_in addr,from;
    socklen_t  len=sizeof(from);
    char   buf[SIZE+1];
    struct client  cinfo;

    pthread_t tid;

    //create socket
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==-1)
    {
        perror("create socket");
        return 1;
    }
    //bind:服务为被动模式 
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(SER_PORT);
    addr.sin_addr.s_addr=INADDR_ANY;//inet_addr(SER_IP);
    memset(addr.sin_zero,0,8);
    ret=bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
        perror("bind");
        shutdown(sockfd,SHUT_RDWR);
        return 2;
    }
    printf("server start success.\n");

    //等待客户端的请求连接
    while(1)
    {
       ret=recvfrom(sockfd,buf,SIZE,0,(struct sockaddr *)&from,&len);
       if(ret==0||ret==-1)  continue;

       buf[ret]='\0';//约定请求连接指令为"CONNECT"
       if(strcmp(buf,"CONNECT")!=0)
       {
           //约定指令不对 发空数据报表连接拒绝
           sendto(sockfd,"Connection refused",strlen("Connection refused"),0,(struct sockaddr *)&from,sizeof(from));
           sendto(sockfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
           continue;
       }
       //请求连接正确 建立进程或线程服务(发数据)
       cinfo.sfd =sockfd;
       cinfo.dest=from;
       ret=pthread_create(&tid,NULL,doServer,&cinfo);
       if(ret!=0)
       {
           sendto(sockfd,"sorry,server busy!",strlen("sorry,server busy!"),0,(struct sockaddr *)&from,sizeof(from));
           sendto(sockfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
       }

       printf("connect from %s:%d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
       //防止服务线程执行cc=*(struct client *)arg语句前 新的连接到来造程cinfo竞争
       pthread_mutex_lock(&mm);
       while(flag!=0)
       {
          pthread_cond_wait(&ok,&mm);
       }
       flag=1;
       pthread_mutex_unlock(&mm);
    }


    return 0;
}
