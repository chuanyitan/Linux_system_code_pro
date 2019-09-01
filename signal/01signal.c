
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigFunc(int signo)
{
    printf("a sig recv signo:%d\n",signo);

    if(signo==SIGFPE)
    {
        printf("不应该除0操作.\n");
        exit(1);
    }
    else if(signo==SIGSEGV)
    {
        printf("访问非法内存.\n");
        exit(2);
    }
}

int main(void)
{
     int value=10;
     int *ptr=NULL;

     signal(SIGFPE,sigFunc);
     signal(SIGSEGV,sigFunc);

     //value=value/0;//除零触发SIGFPE信号

     *ptr=100;


     printf("====main over=====\n");

     return 0;
}
