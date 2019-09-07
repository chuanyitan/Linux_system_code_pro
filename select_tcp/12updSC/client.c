
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#define        SER_PORT       10086
#define        SER_IP         "192.168.2.122"
#define        SIZE           1024

int myconnect(int sfd,struct sockaddr *addr,socklen_t  len);

int main(void)
{
    int sockfd,ret;
    struct sockaddr_in ser_addr;
    char   buf[SIZE+1],msg[SIZE+1];

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

    ret=myconnect(sockfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
    if(ret!=0)
    {
        fprintf(stderr,"连接被拒绝.\n");
        shutdown(sockfd,SHUT_RDWR);
        return 2;
    }

    fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);
    //接收数据
    printf("输入表达式:");
    int count=0;
    while(1)
    {
       if(scanf("%s",buf)>0)//-1 O_NONBLOCK
       {
           //检查表达合法性问题!!!!!!
           if(strcmp(buf,"exit")==0)
           {
               send(sockfd,NULL,0,0);//结束
               break;
           }
           ret=send(sockfd,buf,strlen(buf),0);
           if(ret==-1)
           {
              fprintf(stderr,"服务器失去连接.\n");
              break;
           }
       }

       //接收结果
       ret=recvfrom(sockfd,msg,SIZE,MSG_DONTWAIT,NULL,NULL);
       if(ret==0)  break;
       else if(ret>0)
       {
           count=0;//对方在线
           msg[ret]='\0';
           if(strcmp(msg,"hi")==0)//询问消息  <心跳包>
           {
               send(sockfd,"ok",2,0);
           }
           else
           {
               printf("%s=%d\n",buf,*(int *)msg);
           }
       }
       else if(ret==-1)
       {
            count++;
            if(count>=3000)//3s
            {
                printf("和服务器失去连接.\n");
                break;
            }
       }
       //
       usleep(1000);
    }

    shutdown(sockfd,SHUT_RDWR);
    return 0;
}
int myconnect(int sfd,struct sockaddr *addr,socklen_t  len)
{
    int  count=0,ret;
    char buf[SIZE];
    struct sockaddr_in from;
    socklen_t  ll=sizeof(from);
    //发送连接请求
    sendto(sfd,"CONNECT",7,0,addr,len);

    while(1)
    {
       ret=recvfrom(sfd,buf,SIZE,MSG_DONTWAIT,(struct sockaddr *)&from,&ll);
       if(ret==0)  return -1;
       if(ret>0)    break;

       usleep(100000);//0.1s
       count++;
       if(count>=10)  return -1;//超时
    }

    buf[ret]='\0';
    if(strcmp(buf,"ACK")!=0)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        return -1;
    }
    //用sfd连接服务端新套接字的地址信息
    ret=connect(sfd,(struct sockaddr *)&from,sizeof(from));
    if(ret==-1)
    {
        sendto(sfd,NULL,0,0,(struct sockaddr *)&from,sizeof(from));
        return -1;
    }

    //回复确认信息
    ret=send(sfd,"SYN",3,0);
    if(ret==-1)
    {
        send(sfd,NULL,0,0);
        return -1;
    }

    return 0;
}
