
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
     my.sin_addr.s_addr=inet_addr("192.168.2.75");
     memset(my.sin_zero,0,8);
     ret=bind(sockfd,(void *)&my,sizeof(my));
     if(ret==-1)
     {
         perror("bind");
         return 2;
     }

     int count=0;
     char buf[1024+1];
     struct sockaddr_in   from;
     socklen_t  len=sizeof(from);
     while(1)
     {
        //MSG_TRUNC:当数据报实际长度大于 接收size  则截短数据报 返回数据报的实际长度
        ret=recvfrom(sockfd,buf,10,MSG_TRUNC,(struct sockaddr *)&from,&len);
        if(ret==0)//sendto 发送空数据报
        {
            printf("通信结束.\n");
            break;
        }
        else if(ret==-1)
        {
            perror("revfrom");
            usleep(100000);
            continue;
        }
        if(ret>10){//说明取的数据 被截短
           buf[10]='\0';
        }else{
           buf[ret]='\0';
        }
        printf("from:%s:%d msg:%s   ret:%d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port),buf,ret);
     }


     shutdown(sockfd,SHUT_RDWR);
     return 0;
}
