//异步回收子进程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
/*
 * bugs:进程如果停止stop 收到SIGCHLD信号  wait则阻塞 从而 父进程也无法正常作业
 * sigaction SA_NOCLDSTOP 进程停止时不发生SIGCHLD信号 解决以上bugs
 * 新的bugs:如果人为发送一个SIGCHLD信号给父进程  则wait阻塞
*/
void waitChild(int signo)
{
     printf("a sig recv:%d\n",signo);
     if(signo!=SIGCHLD)  return;

     printf("wait child:%d\n",wait(NULL));
}


int main(void)
{
     pid_t pid;

     struct sigaction  ac={.sa_handler=waitChild};
     sigemptyset(&ac.sa_mask);//不期望屏蔽其它信号
     ac.sa_flags=SA_NOCLDSTOP;//进程停止时不发生SIGCHLD信号
     sigaction(SIGCHLD,&ac,NULL);

     pid=fork();
     if(pid==-1)   return 1;//error
     else if(pid==0)//child
     {
         printf("child <%d> start work.\n",getpid());
         sleep(60);
         printf("child <%d> end   work.\n",getpid());
         exit(0);
     }
     //parent
     while(1)
     {
         getchar();
         printf("parent working....\n");
     }

     return 0;
}
