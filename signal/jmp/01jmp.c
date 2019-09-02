//长跳转语句:只能从上层栈跳回到下层栈
#include <stdio.h>
#include <setjmp.h>

static jmp_buf env1;
static jmp_buf env2;

void func1(void);
void func2(void);
void func3(void);

int main(void)
{
    int ret;
    //记录跳转点
    ret=setjmp(env1);
    printf("main setjmp ret:%d\n",ret);
    if(ret==0)//记录跳转点 成功
    {
       func1();
    }
    else if(ret==110)
    {
        printf("jmp from func3 to main\n");
    }

    return 0;
}
void func1(void)
{
    int ret;
    printf("%s start .\n",__func__);
    ret=setjmp(env2);
    if(ret==0){
       func2();
    }else{
       printf("jmp from func3 to func1\n");
    }
    printf("%s return.\n",__func__);

}
void func2(void)
{
    printf("%s start .\n",__func__);
    func3();
    printf("%s return.\n",__func__);
}
void func3(void)
{
    int  value;
    printf("%s start .\n",__func__);

    printf("input int value:");
    scanf("%d",&value);

    if(value==10){//直接跳入到main
       //110用于区分从某个地方跳转
       longjmp(env1,110);
    }else{//跳入到func1
       longjmp(env2,110);
    }
    printf("%s return.\n",__func__);
}
