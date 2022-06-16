#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<semaphore.h>
#include<sys/stat.h>

int main()
{
	int fd[2];
	pid_t p;
	int n;
	int ret;
	int count=0;
	char outbuf[1024];
	sem_t *s;
	s = sem_open("/s",O_CREAT,0644,0);
	
	if(pipe(fd)==-1)
	{
		printf("管道创建失败");
		return 0;
	}
	p=fork();
	if(p==-1)
	{
		printf("进程p创建失败");
		exit(0);
	}
	else if(p==0)
	{
		close(fd[0]);
		while(1)
		{
			ret=write(fd[1],"*",1);
		    if(ret==-1)
		        break;
		    count++;
		    printf("count=%d\n",count);
		}
		printf("count=%d\n",count);
		sem_post(s);
		exit(1);
	}
	else
	{
		sem_wait(s);
		close(fd[1]);
		n=read(fd[0],outbuf,1024);
		printf("父进程从管道中读取的文件大小为：%d\n",n);
		puts(outbuf);
	}
	
	sem_close(s);
	sem_unlink("/s");
	return 0;

}
