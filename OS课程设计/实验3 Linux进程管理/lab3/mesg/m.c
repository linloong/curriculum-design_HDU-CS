#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<semaphore.h>

struct msgbuf
{
	long mtype;  //消息类型：1 普通消息; 2 rcv对s1应答消息；3 rcv对s2应答消息
	char mtext[1024];
};

sem_t mutex,r1,r2,f1,f2;
pthread_t s1,s2,rcv;
int flag1=0;
int flag2=0;
int msgid;

void *sender1()
{
	struct msgbuf msg;
	msgid = msgget(IPC_PRIVATE,IPC_CREAT|0666);
	if(msgid==-1)
	{
		printf("创建消息队列出错\n");
		exit(1);
	}
	while(1)
	{
		msg.mtype = 1;
		sem_wait(&mutex);
		printf("sender1 send: ");
		gets(msg.mtext);
		if(strcmp(msg.mtext,"exit")==0)
		{
			strcpy(msg.mtext,"end1");
			printf("sender1 send: end1\n");
			msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
			sem_post(&mutex);
			break;
		}
		msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
		sem_post(&mutex);
	}
	sem_wait(&r1);
	msgrcv(msgid,&msg,sizeof(struct msgbuf),2,0);
	printf("sender1 rcv: %s\n",msg.mtext);
	sem_post(&f1);
	pthread_exit(NULL);
}

void *sender2()
{
	struct msgbuf msg;
	while(1)
	{
		msg.mtype = 1;
		sem_wait(&mutex);
		printf("sender2 send: ");
		gets(msg.mtext);
		if(strcmp(msg.mtext,"exit")==0)
		{
			strcpy(msg.mtext,"end2");
			printf("sender2 send: end2\n");
			msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
			sem_post(&mutex);
			break;
		}

		msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
		sem_post(&mutex);
	}
	sem_wait(&r2);
	msgrcv(msgid,&msg,sizeof(struct msgbuf),3,0);
	printf("sender2 rcv: %s\n",msg.mtext);
	sem_post(&f2);
	pthread_exit(NULL);
}

void *receive()
{
	struct msgbuf msg;
	while(flag1==0||flag2==0)
	{
		sem_wait(&mutex);
		msgrcv(msgid,&msg,sizeof(struct msgbuf),1,0);
		printf("receive rcv: %s\n",msg.mtext);
		if(strcmp(msg.mtext,"end1")==0)
		{
			msg.mtype = 2;
			strcpy(msg.mtext,"over1");
			printf("receive send: over1\n");
			msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
			flag1=1;
			sem_post(&mutex);
			sem_post(&r1);
		}
		else if(strcmp(msg.mtext,"end2")==0)
		{
			msg.mtype = 3;
			strcpy(msg.mtext,"over2");
			printf("receive send: over2\n");
			msgsnd(msgid,&msg,sizeof(struct msgbuf),0);
			flag2=1;
			sem_post(&mutex);
			sem_post(&r2);
		}
		sem_post(&mutex);
	}
	sem_wait(&f1);
	sem_wait(&f2);
	msgctl(msgid,IPC_RMID,0);
	pthread_exit(NULL);
}

int main()
{
	sem_init(&mutex,0,1);
	sem_init(&r1,0,0);
	sem_init(&r2,0,0);
	sem_init(&f1,0,0);
	sem_init(&f2,0,0);
	
	pthread_create(&s1,NULL,sender1,NULL);
	pthread_create(&s2,NULL,sender2,NULL);
	pthread_create(&rcv,NULL,receive,NULL);
	
	pthread_join(s1,NULL);
    pthread_join(s2,NULL);
    pthread_join(rcv,NULL);

	sem_destroy(&mutex);
	sem_destroy(&r1);
	sem_destroy(&r2);
	sem_destroy(&f1);
	sem_destroy(&f2);
	return 0;
}
