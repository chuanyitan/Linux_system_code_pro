
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

//默认启动脚本
/*
touch ./server_log.log
chmod 777 ./server_log.log
./server  2>&1 1>./server_log.log &
*/

pthread_mutex_t  mm=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   ok=PTHREAD_COND_INITIALIZER;
static   int flag=1;


#define       SER_PORT      55055
#define       SER_IP        "192.168.122.1"

#define       MAX_WAIT      10

char *get_time()
{
    time_t nSeconds;
    struct tm * pTM;
    char psDateTime[1024];
    time(&nSeconds);
    pTM = localtime(&nSeconds);

    /* 系统日期和时间,格式: yyyymmddHHMMSS */
    sprintf(psDateTime, "%04d-%02d-%02d/%02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
            
    return psDateTime;
}



void *doServer(void *arg)
{
	int err,ret,i,maxfd,num,connfd;
	pthread_mutex_lock(&mm);
    connfd=*(int *)arg;
    flag=0;
    pthread_mutex_unlock(&mm);
    pthread_cond_signal(&ok);
	pthread_detach(pthread_self());
	
	char buf[1024];
	fd_set fd_read;
	fd_set fd_select;
	struct timeval timeout;           //超时时间备份
    struct timeval timeout_select;  //用于select
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd_read);
	FD_ZERO(&fd_select);
	FD_SET(connfd, &fd_read);  //将监听套接字加入fd_set
	maxfd = connfd;

	while(1)
        {	
    //每次都需要重新赋值
            fd_select = fd_read;
            timeout_select = timeout;
            //err = select(maxfd+1, &fd_select, NULL, NULL, NULL);     
            err = select(maxfd+1, &fd_select, NULL, NULL, (struct timeval *)&timeout_select);
			printf(" the err is %d\n",err);
            if(err < 0)
            {  
					printf("[%s]fail to select disconnect.\n",get_time());
                    shutdown(connfd,SHUT_RDWR);
                    pthread_exit((void *)0);       
            }
            if(err == 0)
                    printf("[%s]timeout\n",get_time());

            //检测监听套接字是否可读
            //从1开始查看连接套接字是否可读，因为上面已经处理过0（sockfd）

            if(FD_ISSET(connfd, &fd_select))
            {
                    printf("[%s]connfd is  ok\n",get_time());
                    num = read(connfd, buf, 1024);
					if(num ==0)
					{
						printf("[%s]fail to select disconnect.\n",get_time());
                        shutdown(connfd,SHUT_RDWR);
                        pthread_exit((void *)0);   
					}
                    if(num > 0)
                    {
                            //收到 客户端数据并打印
                            buf[num] = '\0';
                            printf("[%s]receive buf from client connfd is: %s\n",get_time(),buf);
                    }
                         //回复客户端
                    num = write(connfd, "ok", sizeof("ok"));
                    if(num <0)
                    {
                         printf("[%s]fail to select disconnect.\n",get_time());
                         shutdown(connfd,SHUT_RDWR);
                         pthread_exit((void *)0);   
                    }
                    else
                    {
                            printf("[%s]send reply\n",get_time());
                    }
            }
            else
            {
                    printf("[%s]no data\n",get_time());                  
            }
        

        }
		shutdown(connfd,SHUT_RDWR);
        pthread_exit((void *)0);

}

int main(void)
{
     int sockfd,ret;
     pthread_t   tid;

   
     signal(SIGPIPE,SIG_IGN);

     sockfd=socket(AF_INET,SOCK_STREAM,0);
     if(sockfd==-1)
     {
        perror("create socket");
        return 1;
     }

     struct sockaddr_in  addr;
     addr.sin_family=AF_INET;
     addr.sin_port  =htons(SER_PORT);
     //addr.sin_addr.s_addr=inet_addr(SER_IP);
     addr.sin_addr.s_addr=INADDR_ANY;
     memset(addr.sin_zero,0,8);
     ret=bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));
     if(ret==-1)
     {
        perror("bind");
        shutdown(sockfd,SHUT_RDWR);
        return 2;
     }
  
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
     
     
        newsfd=accept(sockfd,(struct sockaddr *)&from,&len);
        if(newsfd==-1)  continue;

        printf("[%s]sockfd:%d newsfd:%d from:%s<%d>\n",\
            get_time(),sockfd,newsfd,inet_ntoa(from.sin_addr),ntohs(from.sin_port));

   
        ret=pthread_create(&tid,NULL,doServer,&newsfd);
        if(ret!=0)
        {
            write(newsfd,"sorry,can't server",sizeof("sorry,can't server"));
            shutdown(newsfd,SHUT_RDWR);
            continue;
        }

  
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
