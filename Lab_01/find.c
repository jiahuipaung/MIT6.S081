#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int find(char *curPath, char *targetFile)
{
    int fd;
    char buf[512], *p; //创建缓冲区和指针来保存完整路径和指示当前路径
    struct stat st;
    struct dirent de;
    int found = 0;
    
    if ((fd = open(curPath, O_RDONLY)) < 0) //目录打开失败
    {
        printf("find: cannot open %s\n", curPath);
        return 0;
    }

    if (fstat(fd, &st) < 0)  //获取fd状态
    {
        printf("find: cannot stat %s\n", curPath);
        close(fd);
        return 0;
    }

    strcpy(buf, curPath);
    p = buf + strlen(buf); //指向当前目录路径下的起始位置
    strcpy(p, "/"); //添加路径分割以便打印

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        if (!strcmp(".", de.name) || !strcmp("..", de.name)) //检索到.和..文件
        {
            continue;
        }

        memmove(p+1, de.name, DIRSIZ); //当前文件完整路径
        *(p + DIRSIZ) = 0; //字符串结尾

        if (stat(buf, &st) < 0)  //将该文件stat装入st
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch (st.type)
        {
        case T_FILE:
        case T_DEVICE:
            if (strcmp(de.name, targetFile) == 0) //找到
            {
                printf("%s\n", buf);
                found = 1;
            }
            break;
        
        case T_DIR:
            find(buf, targetFile);
            break;
        }
    }
    close(fd);
    return found;
}


int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("usage: find need 2 args at least, 3 args at most");
        exit(0);
    }
    int found;
    if (argc == 2) // 默认在.目录下查找
    {
        found = find(".", argv[1]);
    }
    if(argc == 3)
    {
        found = find(argv[1], argv[2]);
    }
    if (!found)
    {
        printf("can't find target file!\n");
    }
    exit(0);
}