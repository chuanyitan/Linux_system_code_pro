//可中断的系统调用
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

#if 0
    //实现支持被打断的系统调重启
    signal(SIGINT,sigFunc);
#else
    //struct sigaction  newAction={.sa_handler=sigFunc,.sa_flags=SA_RESTART};
    struct sigaction  newAction={.sa_handler=sigFunc,.sa_flags=SA_INTERRUPT};
    sigaction(SIGINT,&newAction,NULL);
#endif //

    ret=read(STDIN_FILENO,buf,4096);
    if(ret==-1){
       perror("read stdin");
    }else if(ret==0){
       printf("正常结束输入ctrl+d\n");
    }else{
       buf[ret]='\0';
       printf("stdin>>%s\n",buf);
    }

    return 0;
}
