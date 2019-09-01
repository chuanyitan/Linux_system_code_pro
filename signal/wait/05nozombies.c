//如果无需关心进程结束状态 可以设置子进程结束时不产生僵尸进程有内核值接回收
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define     NR         40


int main(void)
{
     pid_t pid;

#if 0
     //子进程结束或停止 或者从停止到继续 发生SIGCHLD信号
     //某些平台将SIGCHLD信号设置忽略行为 则不产生僵尸进程
     if(signal(SIGCHLD,SIG_IGN)==SIG_ERR)
     {
         perror("signal");
         return 2;
     }
#else
     struct sigaction  ac={.sa_handler=SIG_IGN};
     sigemptyset(&ac.sa_mask);//不期望屏蔽其它信号
     ac.sa_flags=SA_NOCLDWAIT;//不产生僵尸进程
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
