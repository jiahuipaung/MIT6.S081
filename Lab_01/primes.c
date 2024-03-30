#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void process(int rfd)
{
    int my_num = 0;
    int forked = 0;
    int pass_num = 0;
    int pipes[2];
    while (1)
    {

        int readBytes = read(rfd, &pass_num, 4);  //每次读取一个数字
        if(readBytes == 0)
        {
            close(rfd); //读到文件末，关闭
            if (forked)
            {
                close(pipes[1]); //关闭写
                int chile_status;
                wait(&chile_status);
            }
            exit(0);
        }
        if(my_num == 0) //读取的第一位，也是该进程的分配的素数
        {
            my_num = pass_num;
            printf("prime %d\n", my_num);
        }
        if (pass_num % my_num != 0) //非整数倍
        {
            if (forked == 0) //第一次创建子进程
            {
                pipe(pipes);
                forked = 1;
                int ret = fork();
                if (ret == 0) //子进程
                {
                    close(pipes[1]); //关闭写，只读
                    close(rfd); //关闭前一个管道
                    process(pipes[0]);
                }
                else //父进程
                {
                    close(pipes[0]); //关闭读，只写
                }
            }
            write(pipes[1], &pass_num, 4);
        }
    }
}
int main()
{
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i <= 35; i++)
    {
        write(pipes[1], &i, 4);
    }
    close(pipes[1]);
    process(pipes[0]);
    exit(0);
    
}