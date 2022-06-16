#ifndef DIRMANGE_H_INCLUDED
#define DIRMANGE_H_INCLUDED

void my_cd(char *dirnm)
{
    char *dir;
    int fd;
    char dirname[50];
    strcpy(dirname,dirnm);
    char *s;
    dir = strtok_r(dirname, "\\",&s);//分解字符串为一组字符串。dirname为要分解的字符串，"\\"为分隔符字符串
    if(strcmp(dir, ".") == 0)
        return;
    else if(strcmp(dir, "..") == 0)
    {
        if(curdir)
            curdir = my_close(curdir);
        return;
    }
    else if(strcmp(dir, "root") == 0)
    {
        while(curdir)
            curdir = my_close(curdir);
        dir = strtok_r(NULL, "\\",&s);
    }
    int flag=curdir;
    while(dir)
    {
        fd =my_open(dir);
        if(fd != -1)
            curdir = fd;
        else
		{
			while((curdir=my_close(curdir))!=flag);
				return;
		}
        dir = strtok_r(NULL, "\\",&s);
    }
}

void my_mkdir(char *dirname)
{
    fcb *fcbptr;
    fat *fat1, *fat2;
    time_t now;
    struct tm *nowtime;
    char text[MAXTEXT];
    unsigned short blkno;
    int rbn, fd, i;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找，是否有重名目录
    {
        if(strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)
        {
            printf("Error,the dirname is already exist!\n");
            return;
        }
        fcbptr++;
    }
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)
    {
        if(fcbptr->free == 0)
            break;
        fcbptr++;
    }
    blkno = findblock();//寻找空闲盘块
    if(blkno == -1)
        return;
    (fat1 + blkno)->id = END;
    (fat2 + blkno)->id = END;
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, dirname);
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = 0x10;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    openfilelist[curdir].count = i * sizeof(fcb);//把当前目录的文件读写指针定位到文件末尾
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);//从指针fcbptr开始写一个fcb大小的内容到当前目录文件末尾

    fd = my_open(dirname);//返回新建立的目录文件在用户打开文件数组的下标
    if(fd == -1)
        return;
    fcbptr = (fcb *)malloc(sizeof(fcb));//建立新目录的'.','..'目录
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, ".");
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = 0x10;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    do_write(fd, (char *)fcbptr, sizeof(fcb), 2);
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, "..");
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = 0x10;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    do_write(fd, (char *)fcbptr, sizeof(fcb), 2);
    free(fcbptr);
    my_close(fd);

    fcbptr = (fcb *)text;
    fcbptr->length = openfilelist[curdir].length;
    openfilelist[curdir].count = 0;
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);//更新当前目录文件的内容
    openfilelist[curdir].fcbstate = 1;
}

void my_rmdir(char *dirname)
{
    fcb *fcbptr,*fcbptr2;
    fat *fat1, *fat2, *fatptr1, *fatptr2;
    char text[MAXTEXT], text2[MAXTEXT];
    unsigned short blkno;
    int rbn, rbn2, fd, i, j;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);
    if(strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)
    {
        printf("Error,can't remove this directory.\n");
        return;
    }
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)//查找要删除的目录
    {
        if(strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)
            break;
        fcbptr++;
    }
    if(i == rbn / sizeof(fcb))
    {
        printf("Error,the directory is not exist.\n");
        return;
    }
    fd = my_open(dirname);//目录在当前打开文件数组中的下标
    rbn2 = do_read(fd, openfilelist[fd].length, text2);//读取要删除的目录的内容
    fcbptr2 = (fcb *)text2;
    for(j = 0; j < rbn2 / sizeof(fcb); j++)//判断要删除目录是否为空
    {
        if(strcmp(fcbptr2->filename, ".") && strcmp(fcbptr2->filename, "..") && strcmp(fcbptr2->filename, ""))
        {
            my_close(fd);
            printf("Error,the directory is not empty.\n");
            return;
        }
        fcbptr2++;
    }
    blkno = openfilelist[fd].first;
    while(blkno != END)//修改要删除目录在fat中所占用的目录项的属性
    {
        fatptr1 = fat1 + blkno;
        fatptr2 = fat2 + blkno;
        blkno = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    my_close(fd);
    strcpy(fcbptr->filename, "");//修改已删除目录在当前目录的fcb的属性
    fcbptr->free = 0;
    openfilelist[curdir].count = i * sizeof(fcb);//更新当前目录文件的内容
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].fcbstate = 1;
}

void my_ls()
{
    fcb *fcbptr;
    char text[MAXTEXT];
    int rbn, i;
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)
    {
        if(fcbptr->free)
        {
            if(fcbptr->attribute == 0x10)
                printf("%s\\\t\t<DIR>\t\t%d/%d/%d\t%02d:%02d:%02d\n", fcbptr->filename, (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x001f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x003f, fcbptr->time & 0x001f * 2);
            else
                printf("%s.%s\t\t%dB\t\t%d/%d/%d\t%02d:%02d:%02d\t\n", fcbptr->filename, fcbptr->exname, (int)(fcbptr->length), (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x1f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x3f, fcbptr->time & 0x1f * 2);
        }
        fcbptr++;
    }
}

#endif // DIRMANGE_H_INCLUDED
