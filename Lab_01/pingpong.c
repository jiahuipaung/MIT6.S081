#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid;
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    char buff[] = "a";

    int ret = fork();

    if (ret == 0) //子进程
    {
        pid = getpid();
        close(pipe1[1]); //pipe1关闭写，只读
        close(pipe2[0]); //pipe2关闭读，只写
        read(pipe1[0], buff, 1);
        printf("%d: received ping\n", pid);
        write(pipe2[1], buff, 1);
        exit(0);
    }
    else  //父进程
    {
        pid = getpid();
        close(pipe1[0]); //pipe1关闭读，只写
        close(pipe2[1]); // pipe2关闭写，只读
        write(pipe1[1], buff, 1);
        wait(0);
        read(pipe2[0], buff, 1);
        printf("%d: received pong\n", pid);
        exit(0);
    }
    

    exit(0);
}