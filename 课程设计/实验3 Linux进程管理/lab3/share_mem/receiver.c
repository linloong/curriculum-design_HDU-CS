#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<semaphore.h>
#define key 1201

sem_t *mutex,*s,*read;
char *shmpt;

void receiver()
{
	char info[20];
	int v;
	memset(info,'\0',sizeof(info));
	while(1)
	{
		sem_wait(read);
		strcpy(info,shmpt);
		printf("receiver rcv: %s\n",info);
		if(strcmp(info,"end")==0)
		{
			strcpy(shmpt,"over");
			printf("receiver send: over\n");
			sem_post(s);
			break;
		}
	}
	printf("receiver end\n");
}

int main()
{
	read = sem_open("/read",O_CREAT,0666,0);
	s = sem_open("/s",O_CREAT,0666,0);
	
	int shmid = shmget(key,256,IPC_CREAT|0666);
	shmpt = shmat(shmid,NULL,0);
	
	receiver();
	
	shmdt(shmpt);
	
	sem_close(s);
	sem_close(read);
	return 0;
}
