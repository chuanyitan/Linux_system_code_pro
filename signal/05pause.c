
#include <stdio.h>
#include <signal.h>

//执行该信号处理函数时 该信号自动的屏蔽  直到函数返回
void sigFunc(int signo)
{
     printf("a sig func Start.<%d>\n",signo);
     sleep(10);
     printf("a sig func End  .<%d>\n",signo);
}

int main(void)
{
     printf("pid:%d\n",getpid());

     if(signal(SIGUSR1,sigFunc)==SIG_ERR)
     {
        return 1;
     }

     if(signal(SIGINT,sigFunc)==SIG_ERR)
     {
        return 1;
     }


     int ret;
     //用于等待信号捕捉返回
     ret=pause();//永远无正确返回 中断的低速系统调用
     if(ret==-1)
     {
        perror("pause");
     }
     printf("====process over====\n");
     return 0;
}
