#ifndef FILEMANAGE_H_INCLUDED
#define FILEMANAGE_H_INCLUDED


unsigned short get_time()
{
	time_t now;
	struct tm *nowtime;
	now = time(NULL);
	nowtime = localtime(&now);
	return nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
}

unsigned short get_data()
{
	time_t now;
	struct tm *nowtime;
	now = time(NULL);
	nowtime = localtime(&now);
	return (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
}


void my_create(char *filenm)
{
    char filename[12];
    strcpy(filename,filenm);
    fcb *fcbptr;
    fat *fat1, *fat2;
    char *fname, *exname, text[MAXTEXT];
    unsigned short blkno;
    int rbn, i;
    time_t now;
    struct tm *nowtime;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + BLOCKSIZE);
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    if(strcmp(fname, "") == 0)
    {
        PRINT_ERROR(FILE_NAME_ERROR);
        return;
    }
    if(!exname)
    {
        PRINT_ERROR(ExNAME_ERROR);
        return;
    }
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)
    {
        if(fcbptr->free==1&&strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
        {
            PRINT_ERROR(FILE_EXIST);
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
    blkno = findblock();
    if(blkno == -1)
        return;
    (fat1 + blkno)->id = END;
    (fat2 + blkno)->id = END;


    strcpy(fcbptr->filename, fname);
    strcpy(fcbptr->exname, exname);
    fcbptr->attribute = 0x00;
    fcbptr->time = get_time();
    fcbptr->date = get_data();
    fcbptr->first = blkno;
    fcbptr->length = 0;
    fcbptr->free = 1;
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);
    fcbptr = (fcb *)text;
    fcbptr->length = openfilelist[curdir].length;
    openfilelist[curdir].count = 0;
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].fcbstate = 1;
}


void my_rm(char *filename)
{
    fcb *fcbptr;
    fat *fat1, *fat2, *fatptr1, *fatptr2;
    char *fname, *exname, text[MAXTEXT];
    unsigned short blkno;
    int rbn, i;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    if(strcmp(fname, "") == 0)
    {
        PRINT_ERROR(FILE_NAME_ERROR);
        return;
    }
    if(!exname)
    {
        PRINT_ERROR(ExNAME_ERROR);
        return;
    }
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)
    {
        if(strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    if(i == rbn / sizeof(fcb))
    {
        PRINT_ERROR(FILE_NOT_EXIST);
        return;
    }
    blkno = fcbptr->first;
    while(blkno != END)
    {
        fatptr1 = fat1 + blkno;
        fatptr2 = fat2 + blkno;
        blkno = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    strcpy(fcbptr->filename, "");
    fcbptr->free = 0;
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].fcbstate = 1;
}


int _is_open(char *fnm,char *exnm,int fd){//判断在目录fd下，是否有一个打开文件
  char fname[8];char exname[4];
  strcpy(fname,fnm);
  strcpy(exname,exnm);
  int i;
 for(i = 0; i < MAXOPENFILE; i++)//在用户打开文件数组查找看当前文件是否已经打开
    {
        if(openfilelist[i].father==fd&&openfilelist[i].topenfile!=0&&strcmp(openfilelist[i].filename, fname) == 0 && strcmp(openfilelist[i].exname, exname) == 0 && i != fd)
        {
            return i;
        }
    }
    return -1;


}
int is_open(char *fname,char *exname){
return _is_open(fname,exname,curdir);

}
int is_exist(char *fnm,char *exnm,int curdir)
{
    char fname[8];char exname[4];
    strcpy(fname,fnm);
    strcpy(exname,exnm);
    fcb *fcbptr;
    char *str,text[MAXTEXT];
    int rbn, fd, i;
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找要打开的文件是否存在
    {
   //     printf("%d %d",sizeof(fcbptr->filename),sizeof(fname));
	    if(fcbptr->free==1&&strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
         return i;
        fcbptr++;
    }
    if(i == rbn / sizeof(fcb))
    {
        printf("Error,the file is not exist.\n");
        return -1;
    }
}



int _my_open(char *filenm,int curdir)
{
    char filename[12];
    strcpy(filename,filenm);
    fcb *fcbptr;
    char *fname, exname[4], *str, text[MAXTEXT];
    int rbn, fd, i;
    fname = strtok(filename, ".");
    str = strtok(NULL, ".");
    if(str)
        strcpy(exname, str);
    else
        strcpy(exname, "");
    if(i=_is_open(filename,exname,curdir)!=-1){
     return i;
    }


    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找要打开的文件是否存在
    {

	    if(fcbptr->free==1&&strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    if(i == rbn / sizeof(fcb))
    {
        PRINT_ERROR(FILE_NOT_EXIST);
        return -1;
    }
    fd = findopenfile();//寻找空闲文件表项
    if(fd == -1)
        return -1;
    strcpy(openfilelist[fd].filename, fcbptr->filename);
    strcpy(openfilelist[fd].exname, fcbptr->exname);
    openfilelist[fd].attribute = fcbptr->attribute;
    openfilelist[fd].time = fcbptr->time;
    openfilelist[fd].date = fcbptr->date;
    openfilelist[fd].first = fcbptr->first;
    openfilelist[fd].length = fcbptr->length;
    openfilelist[fd].free = fcbptr->free;
    openfilelist[fd].dirno = openfilelist[curdir].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].dir, openfilelist[curdir].dir);
    strcat(openfilelist[fd].dir, filename);

    if(fcbptr->attribute == 0x10)
        strcat(openfilelist[fd].dir, "\\");
    openfilelist[fd].father = curdir;
    openfilelist[fd].count = 0;
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].topenfile = 1;
    return fd;
}
int my_open(char *filename){
return _my_open(filename,curdir);
}
int my_close(int fd)
{

    fcb *fcbptr;
    int father;
    if(fd < 0 || fd >= MAXOPENFILE)
    {
        PRINT_ERROR(FILE_NOT_EXIST);

        return -1;
    }

        if(openfilelist[fd].fcbstate)
    {
        fcbptr = (fcb *)malloc(sizeof(fcb));
        strcpy(fcbptr->filename, openfilelist[fd].filename);
        strcpy(fcbptr->exname, openfilelist[fd].exname);
        fcbptr->attribute = openfilelist[fd].attribute;
        fcbptr->time = openfilelist[fd].time;
        fcbptr->date = openfilelist[fd].date;
        fcbptr->first = openfilelist[fd].first;
        fcbptr->length = openfilelist[fd].length;
        fcbptr->free = openfilelist[fd].free;
        father = openfilelist[fd].father;
        openfilelist[father].count = openfilelist[fd].diroff * sizeof(fcb);
        do_write(father, (char *)fcbptr, sizeof(fcb), 2);
        free(fcbptr);

    }
    openfilelist[fd].length=0;
    openfilelist[fd].fcbstate = 0;
    strcpy(openfilelist[fd].filename, "");
    strcpy(openfilelist[fd].exname, "");
    openfilelist[fd].topenfile = 0;
    return openfilelist[fd].father;
}

int my_write(int fd)
{
    fat *fat1, *fat2, *fatptr1, *fatptr2;
    int wstyle, len, ll, tmp;
    char text[MAXTEXT];
    unsigned short blkno;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);
    if(fd < 0 || fd >= MAXOPENFILE)
    {
        PRINT_ERROR(FILE_NOT_EXIST);
        return -1;
    }
    while(1)
    {
        printf("Please enter the number of write style:\n1.cut write\t2.cover write\t3.add write\n");
        scanf("%d", &wstyle);
        if(wstyle > 0 && wstyle < 4)
            break;
        printf("Input Error!");
    }
    getchar();
    switch(wstyle)
    {
        case 1://截断写把原文件所占的虚拟磁盘空间重置为1
            blkno = openfilelist[fd].first;
            fatptr1 = fat1 + blkno;
            fatptr2 = fat2 + blkno;
            blkno = fatptr1->id;
            fatptr1->id = END;
            fatptr2->id = END;
            while(blkno != END)
            {
                fatptr1 = fat1 + blkno;
                fatptr2 = fat2 + blkno;
                blkno = fatptr1->id;
                fatptr1->id = FREE;
                fatptr2->id = FREE;
            }
            openfilelist[fd].count = 0;
            openfilelist[fd].length = 0;
            break;
        case 2:
            openfilelist[fd].count = 0;
            break;
        case 3:
            openfilelist[fd].count = openfilelist[fd].length;
            break;
        default:
            break;
    }
    ll = 0;
    printf("please input write data(end with Ctrl+D):\n");
    while(gets(text))
    {
        len = strlen(text);
        text[len++] = '\n';
        text[len] = '\0';
        tmp = do_write(fd, text, len, wstyle);
        if(tmp != -1)
            ll += tmp;
        if(tmp < len)
        {
            printf("Wirte Error!");
            break;
        }
    }
    return ll;//实际写的字节数
}

int do_write(int fd, char *text, int len, char wstyle)
{
    fat *fat1, *fat2, *fatptr1, *fatptr2;
    unsigned char *buf, *blkptr;
    unsigned short blkno, blkoff;
    int i, ll;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);
    buf = (unsigned char *)malloc(BLOCKSIZE);
    if(buf == NULL)
    {
        PRINT_ERROR(MALLOC_FAILED);
        return -1;
    }
    blkno = openfilelist[fd].first;
    blkoff = openfilelist[fd].count;
    fatptr1 = fat1 + blkno;
    fatptr2 = fat2 + blkno;
    while(blkoff >= BLOCKSIZE)
    {
        blkno = fatptr1->id;
        if(blkno == END)
        {
            blkno = findblock();
            if(blkno == -1)
            {
                free(buf);
                return -1;
            }
            fatptr1->id = blkno;
            fatptr2->id = blkno;
            fatptr1 = fat1 + blkno;
            fatptr2 = fat2 + blkno;
            fatptr1->id = END;
            fatptr2->id = END;
        }
        else
        {
            fatptr1 = fat1 + blkno;
            fatptr2 = fat2 + blkno;
        }
        blkoff = blkoff - BLOCKSIZE;//让blkoff定位到文件最后一个磁盘块的读写位置
    }

    ll = 0;//实际写的字节数
    while(ll < len)//len是用户输入的字节数
    {
        blkptr = (unsigned char *)(myvhard + blkno * BLOCKSIZE);
        for(i = 0; i < BLOCKSIZE; i++)
            buf[i] = blkptr[i];
        for(;blkoff < BLOCKSIZE; blkoff++)
        {
            buf[blkoff] = text[ll++];
            openfilelist[fd].count++;
            if(ll == len)
                break;
        }
        for(i = 0; i < BLOCKSIZE; i++)
            blkptr[i] = buf[i];
        if(ll < len)//如果一个磁盘块写不下，则再分配一个磁盘块
        {
            blkno = fatptr1->id;
            if(blkno == END)
            {
                blkno = findblock();
                if(blkno == -1)
                    break;
                fatptr1->id = blkno;
                fatptr2->id = blkno;
                fatptr1 = fat1 + blkno;
                fatptr2 = fat2 + blkno;
                fatptr1->id = END;
                fatptr2->id = END;
            }
            else
            {
                fatptr1 = fat1 + blkno;
                fatptr2 = fat2 + blkno;
            }
            blkoff = 0;
        }
    }
    if(openfilelist[fd].count > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].count;
    openfilelist[fd].fcbstate = 1;
    free(buf);
    return ll;
}

int my_read(int fd, int len)
{
    char text[MAXTEXT];
    int ll;
    if(fd < 0 || fd >= MAXOPENFILE)
    {
        PRINT_ERROR(FILE_NOT_EXIST);
        return -1;
    }
    openfilelist[fd].count = 0;
    ll = do_read(fd, len, text);//ll是实际读出的字节数
    if(ll != -1)
        printf("%s", text);
    else
        printf("Read Error!\n");
    return ll;
}

int  do_read(int fd, int len, char *text)
{
    fat *fat1, *fatptr;
    unsigned char *buf, *blkptr;
    unsigned short blkno, blkoff;
    int i, ll;
    fat1 = (fat *)(myvhard + BLOCKSIZE);
    buf = (unsigned char *)malloc(BLOCKSIZE);
    if(buf == NULL)
    {
        PRINT_ERROR(MALLOC_FAILED);
        return -1;
    }
    blkno = openfilelist[fd].first;
    blkoff = openfilelist[fd].count;
    if(blkoff >= openfilelist[fd].length)
    {
        puts("Read out of range!");
        free(buf);
        return -1;
    }
    fatptr = fat1 + blkno;
    while(blkoff >= BLOCKSIZE)//blkoff为最后一块盘块剩余的容量
    {
        blkno = fatptr->id;
        blkoff = blkoff - BLOCKSIZE;
        fatptr = fat1 + blkno;
    }
    ll = 0;
    while(ll < len)
    {
        blkptr = (unsigned char *)(myvhard + blkno * BLOCKSIZE);
        for(i = 0; i < BLOCKSIZE; i++)//将最后一块盘块的内容读取到buf中
            buf[i] = blkptr[i];
        for(; blkoff < BLOCKSIZE; blkoff++)
        {
            text[ll++] = buf[blkoff];
            openfilelist[fd].count++;
            if(ll == len || openfilelist[fd].count == openfilelist[fd].length)
                break;
        }
        if(ll < len && openfilelist[fd].count != openfilelist[fd].length)
        {
            blkno = fatptr->id;
            if(blkno == END)
                break;
            blkoff = 0;
            fatptr = fat1 + blkno;
        }
    }
    text[ll] = '\0';
    free(buf);
    return ll;
}

int show_user(){
    printf("show_userfilelist\n----------");
    printf("curdir %d\n",curdir);
	int i;
  for(i = 0; i < MAXOPENFILE; i++)//在用户打开文件数组查找看当前文件是否已经打开
    {
        if(1)
        {
             printf("%d fd :father:dir:%d %d %s len%d\n",openfilelist[i].topenfile,i,openfilelist[i].father,openfilelist[i].dir,openfilelist[i].length);
//printf("get the cur:%d %d %s",curdir,openfilelist[curdir].father,openfilelist[curdir].dir);

        }
    }
      printf("show_userfilelist\n----------");

        return 0;

}


int my_mv(char *filename,char *path)
{
     char *fname, exname[4], *str, text[MAXTEXT],p[50];
     char need_close[10];
     memset(need_close,0,sizeof(need_close));
     fname = strtok(filename, ".");
     str = strtok(NULL, ".");
     if(str)
        strcpy(exname, str);
     else
     {
         PRINT_ERROR(FILE_NO_OPEN);
         return -1;
     }

	//查看该文件有没有被打开
	strcpy(p,path);

	if(is_open(fname,exname)!=-1)
	{
		printf("close the file first\n");
		return -1;
	}

	//如果是根目录的绝对路径
	char *dir;
	int fd=0;
	char *save;
	dir=strtok_r(p,"\\",&save);
	fd=curdir;
	if(strcmp(dir,"root")==0)
	{
		fd=0;
		dir=strtok_r(NULL,"\\",&save);
	}
	fd=0;
	int open=0;
	while(dir)
	{
		char d[8];
		strcpy(d,dir);
		open=_is_open(d,"",fd);
		if(open<0)
		{
			open=fd;
			fd=_my_open(d,fd);
			if(fd<0)
			{
			   while(openfilelist[open].father!=open)
				{
					if(need_close[open]==1&&open>=0)
						open=my_close(open);
					else 
						break;
				}
				return -1;
			}	    
			need_close[fd]=1;
		}
		dir=strtok_r(NULL,"\\",&save);
	}

    int i,rbn;
    fcb *fcbptr1,*fcbptr2;
//在当前目录下寻找该文件
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr1 = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找要打开的文件是否存在
    {
   //     printf("%d %d",sizeof(fcbptr->filename),sizeof(fname));
            if(fcbptr1->free==1&&strcmp(fcbptr1->filename, fname) == 0 && strcmp(fcbptr1->exname, exname) == 0)
            break;
        fcbptr1++;
    }
    if(i == rbn / sizeof(fcb))
    {
        PRINT_ERROR(FILE_NOT_EXIST);
        return -1;
    }
    int old_of=i;
    //在目标路径下寻找该文件是否重名
    openfilelist[fd].count = 0;
    rbn = do_read(fd, openfilelist[fd].length, text);
    fcbptr2 = (fcb *)text;

    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找要打开的文件是否存在
    {
            if(fcbptr2->free==1&&strcmp(fcbptr2->filename, fname) == 0 && strcmp(fcbptr2->exname, exname) == 0)
            break;
        fcbptr2++;
    }
    if(i!= rbn / sizeof(fcb))
    {
        PRINT_ERROR(FILE_EXIST);
        return -1;
    }

//寻找空闲的fcb
    fcbptr2 = (fcb *)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)
    {
        if(fcbptr2->free == 0)
            break;
        fcbptr2++;
    }

	//在目标目录中增加该目录项
	//复制fcb的信息

	strcpy(fcbptr2->filename,fname);
	strcpy(fcbptr2->exname,exname);
	fcbptr2->attribute=fcbptr1->attribute;
	fcbptr2->free=1;
	fcbptr2->time=fcbptr1->time;
	fcbptr2->date=fcbptr1->date;
	fcbptr2->length=fcbptr1->length;
	fcbptr2->first=fcbptr1->first;

	openfilelist[fd].count = i*sizeof(fcb);
	do_write(fd, (char*)fcbptr2, sizeof(fcb), 2);

	fcbptr2 = (fcb *)text;
	fcbptr2->length = openfilelist[fd].length;
	openfilelist[fd].count = 0;
	do_write(fd, (char *)fcbptr2, sizeof(fcb), 2);
	if(fd==0)
	{
		openfilelist[fd].count=sizeof(fcb);
		do_write(fd,(char *)fcbptr2,sizeof(fcb),2);
	}
    openfilelist[fd].fcbstate = 1;
    openfilelist[fd].count=0;
	//show_user();
    //在原来目录中删除该目录项
    strcpy(fcbptr1->filename, "");
    strcpy(fcbptr1->exname, "");
    fcbptr1->free = 0;
    openfilelist[curdir].count =old_of*sizeof(fcb);
    do_write(curdir, (char *)fcbptr1, sizeof(fcb), 2);
    fcbptr1 = (fcb *)text;
    fcbptr1->length=openfilelist[curdir].length;
    openfilelist[curdir].count = 0;
    do_write(curdir, (char *)fcbptr1, sizeof(fcb), 2);
    openfilelist[curdir].fcbstate = 1;


    while(openfilelist[fd].father!=fd)
    {
		if(need_close[fd]==1&&fd>=0)
			fd=my_close(fd);
    	else 
			break;
    }

    return 1;
}

int my_copy(char *filename,char *path)
{
     char *fname, exname[4], *str, text[MAXTEXT],p[50];
     char need_close[10];
     int old_fd,new_fd;
     char *dir;
     int fd=curdir;
     char *save;
     char f[12];
     strcpy(f,filename);
     memset(need_close,0,sizeof(need_close));
     fname = strtok_r(filename, ".",&save);
     str = strtok_r(NULL, ".",&save);
     if(str)
        strcpy(exname, str);
     else
        return -1;

    //查看该文件有没有被打开
    strcpy(p,path);
    old_fd=is_open(fname,exname);
     if(old_fd<0){
     old_fd=my_open(f);
     }
     if(old_fd<0){
        return -1;
     }
	//如果是根目录的绝对路径
     dir=strtok_r(p,"\\",&save);
     fd=curdir;
    if(strcmp(dir,"root")==0){
      fd=0;
      dir=strtok_r(NULL,"\\",&save);
    }

    int open=0;
	while(dir)
	{
		char d[8];
		strcpy(d,dir);
		open=_is_open(d,"",fd);
		if(open<0)
		{
			open=fd;
			fd=_my_open(d,fd);
			if(fd<0)
			{
				my_close(old_fd);
				while(openfilelist[open].father!=open)
				{
					if(need_close[open]==1&&open>=0)
					  open=my_close(open);
					else 
						break;
				}
				return -1;
			}       
			need_close[fd]=1;
		}	
		dir=strtok_r(NULL,"\\",&save);
	}

	openfilelist[old_fd].count=0;
	int len=do_read(old_fd,openfilelist[old_fd].length,text);
	my_close(old_fd);
	int cur=curdir;

	curdir=fd;
	my_create(f);
	new_fd=my_open(f);
	if(new_fd<0)
		return -1;
	need_close[new_fd]=1;
	openfilelist[new_fd].count=0;
	do_write(new_fd,text,len,1);

	while(openfilelist[new_fd].father!=new_fd)
	{
		if(need_close[new_fd]==1&&new_fd>=0)
			new_fd=my_close(new_fd);
		else 
			break;
	}
	curdir=cur;
	return 1;
}

#endif // FILEMANAGE_H_INCLUDED
