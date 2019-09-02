
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define       LOOP       0x0001
#define       ONCE       0x0002
#define       TIME_ERR   -1

typedef  long   TIME_ID;

typedef struct node{
    int   sec;   //定时值
    int   count; //计数器
    int   mode;  //LOOP/ONCE
    void  (*func)(void *);//超时行为
    void  *arg;  //func调用参数

    TIME_ID  id;

    struct node  *prev,*next;
}Timer_t;
static   Timer_t  *head;
////////////////////////////////////////////////////////
void sigFunc(int signo);
int  initTimer(void);
TIME_ID  addTimer(int sec,int mode,void (*func)(void *),void *arg);
void deleteTimer(TIME_ID  id);
void destoryTimer(void);

/////////////////////////////////////////////////////////
void sayHello(void *arg);
void show(void *arg);
void printVal(void *arg);


int main(void)
{
    int ret;
    int val=5566;

    ret=initTimer();
    if(ret!=0)
    {
        fprintf(stderr,"init timer failed.\n");
        return 1;
    }
    //
    TIME_ID  ids[3];
    ids[0]=addTimer(3,LOOP,sayHello,"king");
    if(ids[0]==TIME_ERR)
    {
        fprintf(stderr,"add sayhello timer failed\n");
    }
    ids[1]=addTimer(5,ONCE,show,NULL);
    //if error
    ids[2]=addTimer(10,LOOP,printVal,&val);
    //if error

    printf("\033[31mEnter Del SayHello Timer\033[0m\n");
    getchar();
    deleteTimer(ids[0]);


    
    getchar();
    destoryTimer();
    return 0;
}
int  initTimer(void)
{

    head=malloc(sizeof(Timer_t));
    if(head==NULL)   return -2;

    head->next=head;
    head->prev=head;

    if(signal(SIGALRM,sigFunc)==SIG_ERR)
       return -1;

     alarm(1);//启动系统定时器
     return 0;
}
/*
 *定时检查任务节点是否有超时的
*/
void sigFunc(int signo)
{
     Timer_t  *t,*next;
     
     for(t=head->next;t!=head;t=next)
     {
         next=t->next;
         //
         t->count=t->count+1;//计时
         if(t->count==t->sec)//超时 执行行为
         {
             t->func(t->arg);
             //检查类型
             if(t->mode==LOOP)
             {
                  t->count=0;
             }
             else
             {
                  t->next->prev=t->prev;
                  t->prev->next=t->next;
                  free(t);
             }
         }
     }
     alarm(1);
}
//追加任务节点
TIME_ID addTimer(int sec,int mode,void (*func)(void *),void *arg)
{
     Timer_t  *one;
     one=malloc(sizeof(Timer_t));
     if(one==NULL)   return TIME_ERR;

     one->sec  =sec;
     one->count=0;
     one->mode =mode;
     one->func =func;
     one->arg  =arg;
     one->id   =(TIME_ID)one;

     //
     one->next=head;
     one->prev=head->prev;
     one->next->prev=one;
     one->prev->next=one;
     return one->id;
}

void deleteTimer(TIME_ID  id)
{
     Timer_t  *t,*next;
     for(t=head->next;t!=head;t=next)
     {
         next=t->next;
         if(t->id==id)
         {
            t->next->prev=t->prev;
            t->prev->next=t->next;
            free(t);
         }
     }

}
void destoryTimer(void)
{
     Timer_t  *t,*next;
     alarm(0);
     for(t=head->next;t!=head;t=next)
     {
         next=t->next;
         free(t);
     }
     free(head);
     signal(SIGALRM,SIG_DFL);
}



/////////////////////////////////////////////////////
void sayHello(void *arg)
{
      printf("Hello %s\n",(char *)arg);
}
void show(void *arg)
{
      printf("============show========\n");
}
void printVal(void *arg)
{
      printf("<<<<<val:%d>>>>>>\n",*(int *)arg);
}
