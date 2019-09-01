//可中断的系统调用:超时退出机制
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sigFunc(int signo)
{
    printf("我捕捉到一个信号[%d] 且要从信号函数返回了!\n",signo);
}

int main(void)
{
    char buf[4096];
    int  ret;

    struct sigaction  newAction={.sa_handler=sigFunc,.sa_flags=SA_INTERRUPT};
    sigaction(SIGALRM,&newAction,NULL);

    printf("input you pas in 5s:");
    alarm(5);//设定5s定时器
    ret=scanf("%s",buf);
    if(ret==-1){
       perror("<time out>read stdin");
    }else if(ret==0){
       printf("正常结束输入ctrl+d 放弃输入\n");
       return 1;
    }else{
       alarm(0);//停止定时器
       printf("pas>>%s\n",buf);
    }

    return 0;
}
