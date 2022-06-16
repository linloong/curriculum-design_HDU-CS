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

void sender()
{
	char info[20];
	memset(info,'\0',sizeof(info));
	while(1)
	{
		printf("sender send: ");
		gets(info);
		if(strcmp(info,"exit")==0)
		{
			strcpy(shmpt,"end");
			printf("sender send: end\n");
			sem_post(read);
			break;
		}
		else
		{
			strcpy(shmpt,info);
			sem_post(read);
		}
	}
	sem_wait(s);
	strcpy(info,shmpt);
	printf("sender rcv: %s\n",info);
	printf("sender end\n");
}

int main()
{
	read = sem_open("/read",O_CREAT,0666,0);
	s = sem_open("/s",O_CREAT,0666,0);
	
	int shmid = shmget(key,256,IPC_CREAT|0666);
	shmpt = shmat(shmid,NULL,0);
	
	sender();
	
	shmdt(shmpt);
	shmctl(shmid,IPC_RMID, NULL);
	
	sem_close(read);
	sem_close(s);
	
    sem_unlink("/read");
    sem_unlink("/s");
	return 0;
}
