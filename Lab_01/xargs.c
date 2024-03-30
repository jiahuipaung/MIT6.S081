#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLEN 1024

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("usage: xargs need 3 arguments at least!");
        exit(1);
    }
    
    char *params[MAXARG]; // exec最后要执行的参数
    char buf[MAXLEN];//从std_in读到的参数（管道左侧），以\0隔开
    char ch; //单个字符读取
    int stat; //用以记录读取状态

    for (int i = 1; i < argc; i++) //实际处理的是管道右侧的参数，因此需要将左侧参数补充到后面
    {
        params[i-1] = argv[i]; //保存除xargs外的所有参数
    }

    while (1)
    {
        int index = 0; //buf尾指针
        int arg_begin = 0; //buf中每单个参数的起始位置
        int argv_cnt = argc - 1; //
        while (1)
        {
            stat = read(0, &ch, 1);
            if (stat == 0)
                exit(0);
            if (' ' == ch || '\n' == ch)
            {
                printf("bb");
                buf[index++] = '\0';
                params[argv_cnt++] = &buf[arg_begin];
                arg_begin = index;
                if ('\n' == ch) //遇到\n就停止，运行一次再继续读
                    break;
            }
            else
            {
                buf[index++] = ch;
            }
        }
        params[argv_cnt] = (char*)0; //结束标志
        if (fork() == 0) //子进程
        {
            exec(params[0], params);
        }
        else  //父进程
        {
            wait(0);
        }
    }
    exit(0);
}