#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_PORT 8888
#define SERV_IP "127.0.0.1"
int main(void)
{
        int sockfd;
        int err;
        int connfd;
        struct sockaddr_in serv_addr;
        struct sockaddr_in cli_addr;
        socklen_t serv_len;
        socklen_t cli_len;

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

        //客户端不需要绑定，直接连接即可

        err = connect(sockfd, (struct sockaddr *)&serv_addr, serv_len);
        if(err < 0)

        {
                perror("fail to bind");
                exit(1);
        }

        char buf[1024];
        int num;
        while(1)
        {
                sleep(2);
                num = read(STDIN_FILENO, buf, 1024);
                if(num > 0)
                {

                   //exit（1）代表退出
                        if(strncmp("exit", buf, 4) == 0)
                        {
                                write(sockfd, buf, num);
                                break;
                        }

                }
                        write(sockfd, buf, num);
        }
                num = read(sockfd, buf, 1024);
                if(num > 0)
                {
                        buf[num] = '\0';
                        printf("server reply: %s\n", buf);
                }
                else{
                        printf("error to read\n");

               }

close(sockfd);

return 0;
}