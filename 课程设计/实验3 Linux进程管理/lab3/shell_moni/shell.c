#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main()
{
	int status;
	pid_t pid;
	char* command;
	while(1)
	{
		printf("请输入命令：");
		scanf("%s",command);
		if(strcmp(command,"cmd1") == 0)
		{
			pid = vfork();
			if(pid < 0)
			{
				printf("vfork failed");
				exit(1);
			}
			else if(pid == 0)
				execlp("./cmd1",NULL);
			else
				wait(&status);
		}
		else if(strcmp(command,"cmd2") == 0)
		{
			pid = vfork();
			if(pid < 0)
			{
				printf("vfork failed");
				exit(1);
			}
			else if(pid == 0)
				execlp("./cmd2",NULL);
			else
				wait(&status);
		}
		else if(strcmp(command,"cmd3") == 0)
		{
			pid = vfork();
			if(pid < 0)
			{
				printf("vfork failed");
				exit(1);
			}
			else if(pid == 0)
				execlp("./cmd3",NULL);
			else
				wait(&status);
		}
		else if(strcmp(command,"exit") == 0)
			exit(0);
		else 
			printf("Command not found\n\n");
	}
	return 0;
}
			

