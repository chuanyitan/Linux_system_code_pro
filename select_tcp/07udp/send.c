
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

     //初始化目标主机信息
     struct sockaddr_in  dest;
     dest.sin_family=AF_INET;
     dest.sin_port  =htons(10086);//UDP TCP端口独立不冲突
     dest.sin_addr.s_addr=inet_addr("192.168.2.122");
     memset(dest.sin_zero,0,8);

     int count=0;
     char buf[1024];
     while(count<20)
     {
        snprintf(buf,1024,"%dth udp data.",count++);
        ret=sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&dest,sizeof(dest));
        printf("send to ret:%d\n",ret);
        sleep(1);
     }

     //UDP支持发送空数据报  recvfrom接收时接收到0 有时用于通信结束
     sendto(sockfd,NULL,0,0,(struct sockaddr *)&dest,sizeof(dest));
     shutdown(sockfd,SHUT_RDWR);
     return 0;
}
