# sleep
使用内核函数sleep实现一个用户程序sleep。可以调用的系统函数均位于“user/user.h”中。
做好入参检查和返回即可，直接调用sys sleep。

# pingpong
## 要求
编写一个用户级程序，使用 xv6 系统调用通过一对管道（每个方向一个）在两个进程之间“乒乓”一个字节。父进程应向子进程发送一个字节；子进程应打印“<pid>：已收到 ping”，其中 <pid> 是其进程 ID，将管道上的字节写入到父进程，然后退出；父进程应该从子进程读取字节，打印“<pid>：收到 pong”，然后退出。

## 思路
之前阅读xv6-book是考到过父子进程通过pipe通信的方式，即一方开启读fd，关闭写fd，另一方反之。若要实现双向通信，则需要两个pipe。控制好读写fd以及读写顺序即可。

# primes
## 要求
目标是使用pipe和fork来设置管道。第一个进程将数字 2 到 35 输入管道。对于每个质数，您将安排创建一个进程，通过管道从其左邻居读取数据，并通过另一个管道向其右邻居写入剩余数据。由于xv6的文件描述符和进程数量有限，第一个进程可以停在35个。**基于管道的素筛问题**
## 思路
1. 每个进程被分配一个素数，并将其打印出来；
2. 它从左侧不断读取素数，若该数字为它分配素数的倍数，则过滤掉，否则fork一个新的进程，将该数字分配个新进程；
3. 每个进程都需要等子进程退出后才可结束。

## 报错

### 1.上报无限递归调用错误：
```
user/primes.c:5:6: error: infinite recursion detected [-Werror=infinite-recursion]
    5 | void process(int rfd)
      |      ^~~~~~~
user/primes.c:26:17: note: recursive call
   26 |                 process(pipes[0]);
      |                 ^~~~~~~~~~~~~~~~~
cc1: all warnings being treated as errors
make: *** [user/primes.o] Error 1
```
做如下修改：

> 删除makefile中CFLAGS的-Werror项

### 2.死循环
该题需要有一个标志位forked来标识是否已创建子进程，每个父进程保持只创建一个子进程，后序读取时不再创建。出现死循环是因为创建后未将forked置位。
### 3.uint相关报错
#include顺序问题，具体原因暂未分析，需参照其他源文件引用顺序进行引用。

# find
## 要求
查找当前目录下具有特定名称的所有文件，可使用递归进入子目录

## 思路

### ls读取目录的方式
#### 1.判断一个路径是文件还是目录
1. 通过open打开路径；
2. 通过fstat获取文件状态st；
3. 通过st.type判断
#### 2. 如何递归子目录
如果已知一个fd对应的是一个目录, 我们可以不断读取DIRSIZ大小bytes来迭代每个子文件/子文件目录
```
struct dirent de;
while(read(fd, &de, sizeof(de)) == sizeof(de)) {
  // de.name 得到文件名
  ...
}
```
### .和..
在xv6文件系统中，一个目录下包含.文件和..文件来表示当前目录和上一级目录，需要对这两个文件进行处理，以免进入无限递归。

## 报错
### 1.打印乱码，未找到目标文件
显示乱码应该是访问越界之类的问题

### 2.inum问题
de.inum==0表示这是一块已经初始化并且可以用来创建文件或者文件夹的位置，所以在ls读取的过程中应当无视这一块空间。

### 3.路径中缺少/分割
之前使用*p++赋值/进行分割，但是未生效，导致最后结果不正确。使用strcpy赋值，并在p+1处填入目录下文件名后解决。

# xargs
## 要求
它的参数为要运行的命令，它从标准输入读取行，并为每一行运行该命令，并将该行附加到命令的参数中。
1. 仅需实现向命令传递一个参数
2. 使用fork和exec在每一行上调用命令，父进程用wait等待；
3. 要读取单行输入，一次读取一个字符，直到“\n”
4. kernel/param.h 声明了 MAXARG，如果需要声明 argv 数组，这可能很有用。
5. 文件系统的更改在 qemu 运行期间持续存在；获得干净的文件系统运行make clean进而make qemu。

## 思路
- 参考对于linux上xargs的[详解](https://www.junmajinlong.com/shell/xargs/);
### why xags?
1. 管道可将前者的stdout作为后者的stdin，但是有些命令希望管道传递的是参数，而管道有时无法直接传递到参数位。xargs即实现将管道传过来的stdin处理后作为后面命令的参数。
2. xargs的作用不仅仅限于简单的stdin传递到命令的参数位，它还可以将stdin或者文件stdin分割成批，每个批中有很多分割片段，然后将这些片段按批交给xargs后面的命令进行处理。

### 实现思路
1. 管道已提供了标准输入，需要读取标准输入，分行存储命令

## 报错
1. c语言中单引号和双引号的区别，前者表示字符变量，后者表示字符串变量。混淆导致判断出错，无法正常结束。

