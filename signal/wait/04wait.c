//异步回收子进程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define     NR         40

/*
 * bugs:如果多个进程同时结束 可能造成回收不完全 产生僵尸进程 (同种信号不排队)
 * 循环方式waitpid解决以上bugs
*/
void waitChild(int signo)
{
     pid_t pid;
     printf("a sig recv:%d\n",signo);
     if(signo!=SIGCHLD)  return;

     while(1)
     {
         pid=waitpid(-1,NULL,WNOHANG|WUNTRACED);
         printf("wait child:%d\n",pid);

         if(pid==0||pid==-1)
                break;
     }
}


int main(void)
{
     pid_t pid;

#if 0
     //子进程结束或停止 或者从停止到继续 发生SIGCHLD信号
     if(signal(SIGCHLD,waitChild)==SIG_ERR)
     {
         perror("signal");
         return 2;
     }
#else
     struct sigaction  ac={.sa_handler=waitChild};
     sigemptyset(&ac.sa_mask);//不期望屏蔽其它信号
     ac.sa_flags=SA_NOCLDSTOP;//进程停止时不发生SIGCHLD信号
     sigaction(SIGCHLD,&ac,NULL);
#endif //

     int i;
     for(i=0;i<NR;i++)
     {
        pid=fork();
        if(pid==-1)   return 1;//error
        else if(pid==0)//child
        {
            printf("%dth child <%d> start work.\n",i,getpid());
            sleep(3);
            printf("%dth child <%d> end   work.\n",i,getpid());
            exit(0);
        }
     }
     //parent
     while(1)
     {
         getchar();
         printf("parent working....\n");
     }

     return 0;
}
