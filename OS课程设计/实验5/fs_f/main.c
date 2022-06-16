#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fs.h"
#include "fsManage.h"
#include "dirManage.h"
#include "fileManage.h"

int main()
{
    char cmd[16][10] = {"cd", "mkdir", "rmdir", "ls", "create", "rm", "open", "close", "write", "read", "exit","printfat","move","copy"};
    char s[30], *sp;
    int cmdn, flag = 1, i;
    startsys();
    printf("*********************File System V1.0*******************************\n\n");
    printf("命令名\t\t命令参数\t\t命令说明\n\n");
    printf("cd\t\t目录名(路径名)\t\t切换当前目录到指定目录\n");
    printf("mkdir\t\t目录名\t\t\t在当前目录创建新目录\n");
    printf("rmdir\t\t目录名\t\t\t在当前目录删除指定目录\n");
    printf("ls\t\t无\t\t\t显示当前目录下的目录和文件\n");
    printf("create\t\t文件名\t\t\t在当前目录下创建指定文件\n");
    printf("rm\t\t文件名\t\t\t在当前目录下删除指定文件\n");
    printf("open\t\t文件名\t\t\t在当前目录下打开指定文件\n");
    printf("write\t\t无\t\t\t在打开文件状态下，写该文件\n");
    printf("read\t\t无\t\t\t在打开文件状态下，读取该文件\n");
    printf("close\t\t无\t\t\t在打开文件状态下，关闭该文件\n");
    printf("move\t\t文件名\t目标文件夹\t在关闭状态下，将文件移到目标文件夹\n\n");
    printf("copy\t\t文件名\t目标文件夹\t在关闭状态下，在目标文件夹下建立副本\n\n");
    printf("printfat\t无\t\t\t打印当前fat表\n");
    printf("exit\t\t无\t\t\t退出系统\n\n");
    printf("*********************************************************************\n\n");
    while(flag)
    {
      printf("%s>", openfilelist[curdir].dir);
      gets(s);

//printf("get the fd:%d %d %s\n",fd,openfilelist[fd].father,openfilelist[fd].dir);
//printf("get the root:%d %d %s",0,openfilelist[0].father,openfilelist[0].dir);

      cmdn = -1;
        if(strcmp(s, ""))
        {
            sp=strtok(s, " ");
            for(i = 0; i < 15; i++)
            {
                if(strcmp(sp, cmd[i]) == 0)
                {
                    cmdn = i;
                    break;
                }
            }
            //			printf("%d\n", cmdn);
            switch(cmdn)
            {
                case 0:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                        my_cd(sp);
                    else
                        printf("Please input the right command.\n");
                    break;
                case 1:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                        my_mkdir(sp);
                    else
                        printf("Please input the right command.\n");
                    break;
                case 2:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                        my_rmdir(sp);
                    else
                        printf("Please input the right command.\n");
                    break;
                case 3:
                    if(openfilelist[curdir].attribute == 0x10)
                        my_ls();
                    else
                        printf("Please input the right command.\n");
                    break;
                case 4:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                        my_create(sp);
                    else
                        printf("Please input the right command.\n");
                    break;
                case 5:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                        my_rm(sp);
                    else
                        printf("Please input the right command.\n");
                    break;
                case 6:
                    sp = strtok(NULL, " ");
                    if(sp && (openfilelist[curdir].attribute == 0x10))
                    {
                        if(strchr(sp, '.')){//查找sp中'.'首次出现的位置
			   //printf("\n%s\n",sp);
                            int temp = my_open(sp);
			if(temp!=-1)
				curdir=temp;

			}
                        else
                            printf("the openfile should have exname.\n");
                    }
                    else
                        printf("Please input the right command.\n");
                    break;
                case 7:
                    if(!(openfilelist[curdir].attribute == 0x10))
                        curdir = my_close(curdir);
                    else
                        printf("No files opened.\n");
                    break;
                case 8:
                    if(!(openfilelist[curdir].attribute == 0x10))
                        my_write(curdir);
                    else
                        printf("No files opened.\n");
                    break;
                case 9:
                    if(!(openfilelist[curdir].attribute == 0x10))
                        my_read(curdir, openfilelist[curdir].length);
                    else
                        printf("No files opened.\n");
                    break;
                case 10:
                    if(openfilelist[curdir].attribute == 0x10)
                    {
                        my_exitsys();
                        flag = 0;
                    }
                    else
                        printf("Please input the right command.\n");
                    break;
				case 11:
					my_print_fat();
					break;
				case 12:
					sp=strtok(NULL," ");
					char *p=strtok(NULL," ");
					my_mv(sp,p);

					break;
				case 13:
					sp=strtok(NULL," ");
					p=strtok(NULL," ");
					my_copy(sp,p);
					break;
						default:
							printf("Please input the right command.\n");
							break;
			}
        }
    }
    return 0;
}
