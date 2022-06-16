#ifndef FS_H_INCLUDED
#define FS_H_INCLUDED

#define BLOCKSIZE 1024  // 磁盘块大小
#define SIZE 1024000  // 虚拟磁盘空间大小
#define END 65535  // FAT中的文件结束标志
#define FREE 0  // FAT中盘块空闲标志
#define ROOTBLOCKNUM 2  // 根目录区所占盘块数
#define MAXOPENFILE 10  // 最多同时打开文件个数t
#define MAXTEXT 102400
#define FILE_NOT_EXIST 101
#define MALLOC_FAILED 102
#define FILE_EXIST 103
#define FILE_NAME_ERROR 104
#define ExNAME_ERROR 105
#define FILE_ALREADY_OPEN 106
#define FILE_NO_OPEN 107

//case FILE_NOT_EXIST:printf("Error,the file is not exist.\n");
//    break;
//case MALLOC_FAILED:
//    printf("Error,malloc failed.\n");
//    break;
//case 103:
//    printf("Error,file has exist.\n");
//    break;
//case 104:
//    printf("Error,file must have a right name.\n");
//    break;
//case 105:
//    printf("Error,file must have a extern name.\n");

/* 文件控制块 */
typedef struct FCB{

    char filename[8];  // 文件名
    char exname[3];  // 文件扩展名
    unsigned char attribute;  // 文件属性字段，值为0时表示数据文件，值为0x10时表示目录文件
    unsigned short time;  // 文件创建时间
    unsigned short date;  // 文件创建日期
    unsigned short first;  // 文件起始盘块号
    unsigned int length;  // 文件长度
    char free;  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配
}fcb;

/* 文件分配表 */
typedef struct FAT
{
    unsigned short id;  // 磁盘块的状态（空闲的，最后的，下一个）
}fat;

/* 用户打开文件表 */
typedef struct USEROPEN
{
    char filename[8];  // 文件名
    char exname[3];  // 文件扩展名
    unsigned char attribute;//文件属性字段，值为0时表示目录文件，值为1时表示数据文件
    unsigned short time;  // 文件创建时间
    unsigned short date;  // 文件创建日期
    unsigned short first;  // 文件起始盘块号
    unsigned int length;//文件长度（对数据文件是字节数，对目录文件可以是目录项个数）
    char free;  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配

    unsigned short dirno;  // 相应打开文件的目录项在父目录文件中的盘块号
    int diroff;  // 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号
    char dir[80];  // 相应打开文件所在的路径名，这样方便快速检查出指定文件是否已经打开
    int father;  // 父目录在打开文件表项的位置
    int count;  // 读写指针在文件中的位置,文件的总字符数
    char fcbstate;  // 是否修改了文件的FCB的内容，如果修改了置为1，否则为0
    char topenfile;  // 表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某打开文件占据
}useropen;

/* 引导块 */
typedef struct BLOCK0
{
    char magic[10];  // 文件系统魔数
    char information[200];//存储一些描述信息，如磁盘块大小、磁盘块数量、最多打开文件数等
    unsigned short root;  // 根目录文件的起始盘块号
    unsigned char *startblock;  // 虚拟磁盘上数据区开始位置
}block0;

unsigned char *myvhard;  // 指向虚拟磁盘的起始地址
useropen openfilelist[MAXOPENFILE];  // 用户打开文件表数组
int curdir;  // 用户打开文件表中的当前目录所在打开文件表项的位置
char currentdir[80];  // 记录当前目录的目录名（包括目录的路径）
unsigned char* startp;  // 记录虚拟磁盘上数据区开始位置
char myfilename[] = "myfilesys";//文件系统的文件名

void startsys();  // 进入文件系统
void my_format();  // 磁盘格式化
void my_exitsys();  // 退出文件系统
unsigned short findblock();  // 寻找空闲盘块
int findopenfile();  // 寻找空闲文件表项
void my_print_fat();  //打印当前fat内容

void my_cd(char *dirname);  // 更改当前目录
void my_mkdir(char *dirname);  // 创建子目录
void my_rmdir(char *dirname);  // 删除子目录
void my_ls();  // 显示目录

void my_create (char *filename);  // 创建文件
void my_rm(char *filename);  // 删除文件
int my_open(char *filename);  // 打开文件
int my_close(int fd);  // 关闭文件
int my_write(int fd);  // 写文件
int do_write(int fd, char *text, int len, char wstyle);  // 实际写文件
int my_read (int fd, int len);  // 读文件
int do_read (int fd, int len,char *text);  // 实际读文件
int is_open();
int show_user();
int my_copy(char *filename,char *path);

void PRINT_ERROR(int code_error)
{
    switch (code_error) {
    case FILE_NOT_EXIST:printf("Error,the file is not exist.\n");
        break;
    case MALLOC_FAILED:
        printf("Error,malloc failed.\n");
        break;
    case FILE_EXIST:
        printf("Error,file has exist.\n");
        break;
    case FILE_NAME_ERROR:
        printf("Error,file must have a right name.\n");
        break;
    case ExNAME_ERROR:
        printf("Error,file must have a extern name.\n");
        break;
    case FILE_ALREADY_OPEN:
        printf("Error,file has already open.\n");
        break;
    case FILE_NO_OPEN:
        printf("Error,file do not open.\n");
        break;
    default:
        break;
    }
    return ;
}
#endif // FS_H_INCLUDED
