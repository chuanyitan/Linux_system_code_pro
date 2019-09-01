#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define SERV_PORT 8888
#define SERV_IP "127.0.0.1"         //本地回环接口
#define LIST 20                             //服务器最大接受连接
#define MAX_FD 10                      //FD_SET支持描述符数量
int main(void)
{
        int sockfd;
        int err;
        int i;
        int connfd;

        //下面两个备份原因是select调用后，会发生变化，再次调用select前，需要重新赋值

       fd_set fd_read;                      //FD_SET数据备份

       fd_set fd_select;                    //用于select

       struct timeval timeout;           //超时时间备份
       struct timeval timeout_select;  //用于select

       struct sockaddr_in serv_addr;   //服务器地址
        struct sockaddr_in cli_addr;      //客户端地址
        socklen_t serv_len;
        socklen_t cli_len;

//超时时间设置
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
                perror("fail to socket");
                exit(1);
        }
        memset(&serv_addr, 0, sizeof(serv_addr));
        memset(&cli_addr, 0, sizeof(cli_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERV_PORT);
        serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
        serv_len = sizeof(serv_addr);

        err = bind(sockfd, (struct sockaddr *)&serv_addr, serv_len);
        if(err < 0)
        {
                perror("fail to bind");
                exit(1);
        }

        err = listen(sockfd, LIST);
        if(err < 0)
        {
                perror("fail to listen");
                exit(1);
        }

        char buf[1024];  //读写缓冲区
        int num;
        int maxfd;
        cli_len = sizeof(cli_addr);
		printf("connt.....\n");
        connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
		printf("connected  the connfd is:%d\n",connfd);
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_select);
		FD_SET(connfd, &fd_read);  //将监听套接字加入fd_set
		maxfd = connfd;
		
    while(1)
        {	
    //每次都需要重新赋值
                fd_select = fd_read;
                timeout_select = timeout;
//              err = select(maxfd+1, &fd_select, NULL, NULL, NULL);     
                err = select(maxfd+1, &fd_select, NULL, NULL, (struct timeval *)&timeout_select);
				printf(" the err is %d\n",err);
                if(err < 0)
                {
                        perror("fail to select");
                        exit(1);
                }
                if(err == 0)
                        printf("timeout\n");

                //检测监听套接字是否可读
                //从1开始查看连接套接字是否可读，因为上面已经处理过0（sockfd）

                if(FD_ISSET(connfd, &fd_select))
                {
                        printf("connfd is  ok\n");
                        num = read(connfd, buf, 1024);
                        if(num > 0)
                        {
                                //收到 客户端数据并打印
                                buf[num] = '\0';
                                printf("receive buf from client connfd is: %s\n",  buf);
                        }
                             //回复客户端
                        num = write(connfd, "ok", sizeof("ok"));
                        if(num < 0)
                        {
                                perror("fail to write ");
                                exit(1);
                        }
                        else
                        {
                                printf("send reply\n");
                        }
                }
                else
                {
                        printf("no data\n");                  
                }
          

        }

return 0;
}