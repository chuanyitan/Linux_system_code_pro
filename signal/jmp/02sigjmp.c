
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf  env;

void sigfunc(int signo)
{
     printf("sig func start.\n");
     //该信号没被解除屏蔽
     longjmp(env,120);

     printf("sig func over.\n");
}

int main(void)
{
     int ret;

     signal(SIGINT,sigfunc);

     ret=setjmp(env);
     if(ret==120)
     {
         printf("跳转回来.\n");
     }

     getchar();
     return 0;
}
