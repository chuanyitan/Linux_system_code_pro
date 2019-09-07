
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define        SER_PORT       10086
#define        SER_IP         "192.168.2.122"
#define        SIZE           1024

pthread_mutex_t  mm=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   ok=PTHREAD_COND_INITIALIZER;
static int flag=1;

//示例:客户端发送一个表达式 求结果 返回个客户端(考虑表达是的合法性 由客端保证)
void *doServer(void *arg)
{
    char  buf[SIZE+1];

    pthread_mutex_lock(&mm);
    long  sfd=*(long *)arg;
    flag=0;
    pthread_mutex_unlock(&mm);
    pthread_cond_signal(&ok);

    pthread_detach(pthread_self());

    char  op;
    int   count=0,times=0,ret,i,v1,v2,result;
    while(1)
    {
        //非阻塞 用于定时发送心跳包 测试客户端是否在线  也可用alarm sigsetjmp超时机制       
        ret=recvfrom(sfd,buf,SIZE,MSG_DONTWAIT,NULL,NULL);
        if(ret==0)   break;//0空数据报用于结束
        else if(ret==-1)
        {
            count++;
            usleep(100000);
            if(count%30==0)//3s//确认客户端是否还在线
            {
               send(sfd,"hi",2,0);
               times+=1;
               if(times>=3)   break;//第三次询问 没有收到回复则认为客户失去连接
            }
            continue;
        }
        //激活
        count=0;
        times=0;

        buf[ret]='\0';
        if(strcmp(buf,"ok")==0)//回复在线的信息
          continue;

        //123+10
        for(i=0;buf[i];i++)
        {
            if(buf[i]<'0' || buf[i]>'9')
            {
                op=buf[i];//非数字字符 认为是操作符号
                buf[i]='\0';
                break;
            }
        }
        //123\010
        v1=atoi(buf);
        v2=atoi(buf+i+1);
        switch(op){
           case '+':result=v1+v2;break;
           case '-':result=v1-v2;break;
           case '*':result=v1*v2;break;
           case '/':result=v1/v2;break;
           case '%':result=v1%v2;break;
        }
        //发送结果
        ret=send(sfd,&result,sizeof(int),0);
        if(ret==-1)  break;//connect udp套接字 对方发在 回收到-1返回值
    }

    printf("one client over.\n");
    shutdown(sfd,SHUT_RDWR);
    pthread_exit((void *)0);
}

int main(void)
{
    long sockfd,newsfd,ret;
    struct sockaddr_in addr,from;
    socklen_t  len=sizeof(from);
    char   buf[SIZE+1];

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

    while(1)
    {
       //等待客户端的请求连接
       //newsfd=myaccept(sockfd,NULL,NULL);
       newsfd=myaccept(sockfd,(struct sockaddr *)&from,&len);
       if(newsfd==-1)  continue;//error

       //建立进程或线程服务
       ret=pthread_create(&tid,NULL,doServer,&newsfd);
       if(ret!=0)
       {
           send(newsfd,"sorry,server busy",strlen("sorry,server busy"),0);
           shutdown(newsfd,SHUT_RDWR);
           continue;
       }
       printf("connect from %s:%d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
       //防止服务线程执行sfd=*(long *)arg语句前 新的连接到来造程newsfd竞争
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

int myaccept(int sfd,struct sockaddr *peer,socklen_t  *l)
{
    int  newsfd,ret;
    struct sockaddr_in  from;
    socklen_t   len=sizeof(from);
    char   buf[SIZE];

    //等待连接请求  from为客户端地址
    ret=recvfrom(sfd,buf,SIZE,0,(struct sockaddr *)&from,&len);
    if(ret==0||ret==-1)  return -1;//error

    buf[ret]='\0';//约定请求连接指令为"CONNECT"
    if(strcmp(buf,"CONNECT")!=0)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        return -1;
    }
    //建立 新的套接字用户与客户端通信
    newsfd=socket(AF_INET,SOCK_DGRAM,0);
    if(newsfd==-1)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        return -1;
    }
    //连接newsfd专门与客户from连接  之后直接send(newsfd,,,)方式通信
    ret=connect(newsfd,(struct sockaddr *)&from,sizeof(from));
    if(ret==-1)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        shutdown(newsfd,SHUT_RDWR);
        return -1;
    }
    //发送确认信息个客户端并等待回复
    if(send(newsfd,"ACK",3,0)==-1)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        shutdown(newsfd,SHUT_RDWR);
        return -1;
    }
    //等待回复
    int count=0;
    while(1)
    {
       ret=recvfrom(newsfd,buf,SIZE,MSG_DONTWAIT,NULL,NULL);
       if(ret>0)   break;
       else if(ret==0)
       {
           sendto(newsfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
           shutdown(newsfd,SHUT_RDWR);
           return -1;
       }
       
       usleep(100000);//0.1
       count++;
       if(count>=20)//2s time out
       {
           sendto(newsfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
           shutdown(newsfd,SHUT_RDWR);
           return -1;
       }
    }

    buf[ret]='\0';
    if(strcmp(buf,"SYN")!=0)
    {
        sendto(newsfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        shutdown(newsfd,SHUT_RDWR);
        return -1;
    }

    if(peer!=NULL)
    {
       *(struct sockaddr_in *)peer=from;
    }
    //connect success
    return newsfd;
}



