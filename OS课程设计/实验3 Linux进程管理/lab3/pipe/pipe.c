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
	pid_t p1,p2,p3;
	char *buf;
	char outbuf[100];
	int n;
	int i;
	sem_t *mutex,*s1,*s2,*s3;
	
	mutex = sem_open("/mutex",O_CREAT,0644,1);
	s1 = sem_open("/s1",O_CREAT,0644,0);
	s2 = sem_open("/s2",O_CREAT,0644,0);
	s3 = sem_open("/s3",O_CREAT,0644,0);

	if(pipe(fd)==-1)
	{
		printf("管道创建失败");
		return 0;
	}
	p1=fork();
	if(p1==-1)
	{
		printf("进程p1创建失败");
		exit(0);
	}
	else if(p1==0)
	{
		close(fd[0]);
		buf = "Hello, this is p1\n";
		sem_wait(mutex);
		n=write(fd[1],buf,strlen(buf));
		printf("子进程p1写入完毕，进程写入的内容是：%s\n\n",buf);
		sem_post(mutex);
		sem_post(s1);
		exit(1);
	}
	p2=fork();
	if(p2==-1)
	{
		printf("进程p2创建失败");
		exit(0);
	}
	else if(p2==0)
	{
		close(fd[0]);
		buf = "Hello, this is p2\n";
		sem_wait(mutex);
		n=write(fd[1],buf,strlen(buf));
		printf("子进程p2写入完毕，进程写入的内容是：%s\n\n",buf);
		sem_post(mutex);
		sem_post(s2);
		exit(1);
	}
	p3=fork();
	if(p3==-1)
	{
		printf("进程p3创建失败");
		exit(0);
	}
	else if(p3==0)
	{
		close(fd[0]);
		buf = "Hello, this is p3\n";
		sem_wait(mutex);
		n=write(fd[1],buf,strlen(buf));
		printf("子进程p3写入完毕，进程写入的内容是：%s\n\n",buf);
		sem_post(mutex);
		sem_post(s3);
		exit(1);
	}
	else
	{
		sem_wait(s1);
		sem_wait(s2);
		sem_wait(s3);
		close(fd[1]);
		sem_wait(mutex);
		n=read(fd[0],outbuf,1024);
        sem_post(mutex);
		printf("父进程从管道中读取的文件大小为：%d\n",n);
		printf("父进程从管道读取的内容为：\n");
		puts(outbuf);
	}

	sem_close(mutex);
	sem_close(s1);
	sem_close(s2);
	sem_close(s3);
	sem_unlink("/mutex");
	sem_unlink("/s1");
	sem_unlink("/s2");
	sem_unlink("/s3");
	return 0;

}
