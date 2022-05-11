#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
//malloc 申请内存空间
//如果申请x g    物理内存+虚拟内存 > x则可以申请成功
//swap空间，由物理磁盘映射出的虚拟内存，可能速度慢些
int main(int argc, char **argv)
{
    // 此时实际上只是在虚拟内存空间划分出一块，给s使用，但还未真正分配。
    // 即此时不会占内存空间，只有在这边内存使用后，才是真正的使用了这块内存
    char *s = (char *) malloc(1024*1024*1024);
    assert( s != NULL );
    // 此时才使用到，才真正分配空间
    memset(s, 1, 1024*1024*1024);
    // for (int i = 0; i < (1024*1024*1024); i++) {
    //     s[i] =i;
    // }
    // 虽然没有free，但进程结束了，被进程回收
    printf("main over");

    getchar();

    exit(0);
}

