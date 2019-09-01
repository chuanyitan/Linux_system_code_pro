//setitimer  alarm
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void timeout(int signo)
{
    printf("time out...\n");

    alarm(1);
}

int main(void)
{

    int  ret;
    
    signal(SIGALRM,timeout);
    //超时触发SIGALRM信号
    ret=alarm(10);
    printf("ret:%d\n",ret);

    sleep(3);
    //返回之前定时器所剩余的时间值   说明一个进程指有唯一的一个定时器
    ret=alarm(4);
    printf("ret:%d\n",ret);

    getchar();
    return 0;
}
