
#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static jmp_buf  env;

void sigfunc(int signo)
{
    if(signo==SIGALRM)
    {
        siglongjmp(env,100);
    }
}

int main(void)
{
     char buf[128]={0};


     if(sigsetjmp(env,1)==0)
     {
        signal(SIGALRM,sigfunc);
        alarm(5);
        printf("input you pas in 5s:");
        fgets(buf,128,stdin);
        printf("pas:%s\n",buf);

        alarm(0);
        signal(SIGALRM,SIG_DFL);
     }
     else
     {
         printf("sorry,time out!\n");
         signal(SIGALRM,SIG_DFL);
     }


     getchar();
     printf("do over\n");
     return 0;
}
