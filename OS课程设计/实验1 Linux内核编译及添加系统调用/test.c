#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#define __NR_mysetnice 436

int main()
{
    pid_t pid;
    int nicevalue;
    int flag;
    int prio = 0;
    int nice = 0;
    printf("请输入进程的pid：");
    scanf("%d",&pid);
    while(1)
    {
        printf("请选择使用方式（0表示读取nice值，1表示修改nice值，-1退出程序）:\n");
        scanf("%d",&flag);
        if(flag == -1)
			break;
        else if(flag == 0)
		{
			syscall(__NR_mysetnice,pid,flag,0,&prio,&nice);
			printf("pid: %d  prio: %d  nice: %d\n",pid,prio,nice);
		}
		else if(flag == 1)
		{
			printf("请输入要设置的新nice值:");
			scanf("%d",&nicevalue);
			syscall(__NR_mysetnice,pid,flag,nicevalue,&prio,&nice);
			printf("pid: %d  prio: %d  nice: %d\n",pid,prio,nice);
		}
		else
			printf("输入有误\n");
	}
    return 0;
}