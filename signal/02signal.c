
#include <stdio.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

void sayHello(int signo)
{
    printf("hello hello:%d\n",signo);
}
void func(int signo)
{
    printf("func.......\n");
}

int main(void)
{
    sighandler_t  ret;

    printf("Enter Set SIGINT sayHello Action");
    getchar();
    //返回之前的信号行为函数指针
    ret=signal(SIGINT,sayHello);
    if(ret==SIG_ERR)
    {
        fprintf(stderr,"set SIGINT  sayHello action failed\n");
        return 1;
    }
    else if(ret==SIG_DFL)
    {
         printf("之前SIGINT为默认行为.\n");
    }
    
    printf("Enter Set SIGINT Func Action");
    getchar();
    ret=signal(SIGINT,func);
    if(ret==SIG_ERR){
         printf("signal error");
         return 2;
    }else if(ret==sayHello){
         printf("之前SIGINT为sayHello行为.\n");
    }

    printf("Enter Set SIGINT 忽略 Action");
    getchar();
    ret=signal(SIGINT,SIG_IGN);
    if(ret==SIG_ERR)  return 3;//error
    else if(ret==func){
         printf("之前SIGINT为func行为.\n");
    }
    
    printf("Enter Set SIGINT 默认 Action");
    getchar();
    ret=signal(SIGINT,SIG_DFL);
    if(ret==SIG_ERR)  return 4;
    else if(ret==SIG_IGN){
         printf("之前SIGINT为 忽略 行为.\n");
    }
    
    printf("Enter End Process\n");
    getchar();
    return 0;
}
