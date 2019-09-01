//异步回收子进程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
/*
 * bugs:进程如果停止stop 收到SIGCHLD信号  wait则阻塞 从而 父进程也无法正常作业
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

     //子进程结束或停止 或者从停止到继续 发生SIGCHLD信号
     if(signal(SIGCHLD,waitChild)==SIG_ERR)
     {
         perror("signal");
         return 2;
     }

     pid=fork();
     if(pid==-1)   return 1;//error
     else if(pid==0)//child
     {
         printf("child <%d> start work.\n",getpid());
         sleep(10);
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
