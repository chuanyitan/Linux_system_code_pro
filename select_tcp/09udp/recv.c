//实现双向通信 收到数据 确认  收到确认在发下一个数据 
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
     int sockfd,ret;

     sockfd=socket(AF_INET,SOCK_DGRAM,0);
     if(sockfd==-1)
     {
        perror("create socket");
        return 1;
     }

     //绑定 在对应的IP PORT接收数据
     struct sockaddr_in  my;
     my.sin_family=AF_INET;
     my.sin_port  =htons(10086);//UDP TCP端口独立不冲突
     my.sin_addr.s_addr=inet_addr("192.168.2.122");
     memset(my.sin_zero,0,8);
     ret=bind(sockfd,(void *)&my,sizeof(my));
     if(ret==-1)
     {
         perror("bind");
         return 2;
     }

     int  count=0;
     char buf[1024+4+1];
     struct sockaddr_in   from;
     socklen_t  len=sizeof(from);
     while(1)
     {
        //ret=recvfrom(sockfd,buf,1024,0,NULL,NULL);//NULL不关心消息的来源
        ret=recvfrom(sockfd,buf,1024,0,(struct sockaddr *)&from,&len);
        if(ret==0)
        {
            printf("通信结束.\n");
            break;
        }
        else if(ret==-1)
        {
            perror("revfrom");
            break;
        }
        buf[ret]='\0';

        //查看是否重复的包
        if(*(int *)buf!=count)//不是当前要的包(可能为重复包)
        {
            printf("\033[31m %dth 这不是我要的包\033[0m\n",*(int *)buf);
        }
        else//正确包
        {
            count=*(int *)buf;
            printf("from:%s:%d %dth msg:%s\n",\
               inet_ntoa(from.sin_addr),ntohs(from.sin_port),count,buf+4);

            count++;//正确包 请求下一个包
        }
        sleep(1);
        //确认:
        *(int *)buf=count;
        sendto(sockfd,buf,4,0,(struct sockaddr *)&from,sizeof(from));
     }


     shutdown(sockfd,SHUT_RDWR);
     return 0;
}
