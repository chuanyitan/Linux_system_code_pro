
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf  env;

void sigfunc(int signo)
{
     printf("sig func start.\n");
     //该信号没被解除屏蔽
     siglongjmp(env,120);

     printf("sig func over.\n");
}

int main(void)
{
     int ret;

     signal(SIGINT,sigfunc);

     //setjmp(env)  == sigsetjmp(env,0)
     ret=sigsetjmp(env,1);//1：非0值，则保存当前进程信号屏蔽字 跳转回来时恢复
     if(ret==120)
     {
         printf("跳转回来.\n");
     }

     getchar();
     return 0;
}
