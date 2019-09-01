
#include <stdio.h>
#include <signal.h>

void getSigProcMask(const char *str)
{
     sigset_t  old;

     printf("%s:",str);
     sigprocmask(SIG_SETMASK,NULL,&old);

/*
     if(sigismember(&old,SIGINT))
     {
         printf("当前屏蔽SIGINT\n");
     }
*/
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
     sleep(10);
     printf("a sig func End  .<%d>\n",signo);
}
void sayHello(int signo)
{
     printf("hello start.\n");
     sleep(2);
     printf("hello end.\n");
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


/*
 *   //如此时发生2号信号 则递送阻塞 处于未决状态 可以修改信号行为
 *   信号行为并不实在产生时决定 而是在递送时
     printf("Enter Change Sigint Action.\n");
     getchar();
     signal(SIGINT,sayHello);
*/


     printf("Enter 在原来基础上再屏蔽7 8\n");
     getchar();
     sigset_t  set_2;
     sigemptyset(&set_2);
     sigaddset(&set_2,7);
     sigaddset(&set_2,8);
     //SIG_BLOCK:set_2为想要屏蔽的集合 不影响进程之前的
     ret=sigprocmask(SIG_BLOCK,&set_2,NULL);
     if(ret==-1)    return 33;

     getSigProcMask("SIG_SETMASK再设置7 8时");


     //解除2 7屏蔽
     printf("Enter 解除2 7屏蔽\n");
     getchar();
     sigset_t set_3;
     sigemptyset(&set_3);
     sigaddset(&set_3,2);
     sigaddset(&set_3,7);
     //SIG_UNBLOCK:不影响其它 仅解除set_3集合中的信号
     sigprocmask(SIG_UNBLOCK,&set_3,NULL);
     getSigProcMask(" 解除2 7屏蔽时");


     //恢复信号屏蔽
     printf("Enter 恢复信号屏蔽\n");
     getchar();

     ret=sigprocmask(SIG_SETMASK,&old,NULL);
     if(ret==-1)   return 11;//error
     getSigProcMask("恢复信号屏蔽时");
     


     //用于等待信号捕捉返回
     ret=pause();//永远无正确返回 中断的低速系统调用
     if(ret==-1)
     {
        perror("pause");
     }
     printf("====process over====\n");
     return 0;
}
