
#include <stdio.h>
#include <signal.h>

void getSigProcMask(const char *str)
{
     sigset_t  old;

     printf("%s:",str);
     sigprocmask(SIG_SETMASK,NULL,&old);

     int signo;
     //查看前30号信号
     for(signo=1;signo<=30;signo++)
     {
         if(sigismember(&old,signo))
         {
             printf("%d ",signo);
         }
     }
     putchar('\n');
}


//执行该信号处理函数时 该信号自动的屏蔽  直到函数返回
void sigFunc(int signo)
{
     printf("a sig func Start.<%d>\n",signo);

     getSigProcMask("\033[31m在信号处理程序时\033[0m");      
     sleep(5);
     printf("a sig func End  .<%d>\n",signo);
}


////////////////////////////////////////////
int main(void)
{
     int ret;
     printf("pid:%d\n",getpid());

     //获取进程当前信号屏蔽字
     getSigProcMask("最开始时");

     if(signal(SIGUSR1,sigFunc)==SIG_ERR)
     {
        return 1;
     }

     if(signal(SIGINT,sigFunc)==SIG_ERR)
     {
        return 1;
     }

     printf("Enter Set Sig Mask:1 2 3 4 5\n");
     getchar();
     //把信号12345加入信号集合set
     sigset_t  set,old;
     sigemptyset(&set);
     sigaddset(&set,1);
     sigaddset(&set,2);
     sigaddset(&set,3);
     sigaddset(&set,4);
     sigaddset(&set,5);
     //设置进程屏蔽字
     //ret=sigprocmask(SIG_SETMASK,&set,NULL)
     ret=sigprocmask(SIG_SETMASK,&set,&old);
     if(ret==-1)   return 11;//error

     getSigProcMask("SIG_SETMASK设置屏蔽1 2 3 4 5时");



     //恢复信号屏蔽
     printf("Enter 恢复信号屏蔽\n");
     getchar();

#if 1
     //将进程屏蔽字设置为old（当前old为之前的状态 实际为解除屏蔽）且原子性等待某信号从捕捉函数返回
     sigsuspend(&old);
#else
     //sigprocmase解除屏蔽 未决信号原子性递送 pause函数根本来不及安装(执行) 更别谈中断
     ret=sigprocmask(SIG_SETMASK,&old,NULL);
     if(ret==-1)   return 11;//error
     getSigProcMask("恢复信号屏蔽时");
     
     //用于等待信号捕捉返回
     ret=pause();//永远无正确返回 中断的低速系统调用
     if(ret==-1)
     {
        perror("pause");
     }
#endif //
     printf("====process over====\n");
     return 0;
}
