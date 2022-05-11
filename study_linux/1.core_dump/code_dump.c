#include <stdio.h> 
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#define BACKTRACE_SIZE   16

//https://codeantenna.com/a/MeSMFHURri
//https://blog.csdn.net/Dancer__Sky/article/details/100557754 
//https://codeantenna.com/a/oHFSiprggt

void dump(void)
{
        int j, nptrs;
        void *buffer[BACKTRACE_SIZE];
        char **strings;
        
        nptrs = backtrace(buffer, BACKTRACE_SIZE);
        
        printf("backtrace() returned %d addresses\n", nptrs);
 
        strings = backtrace_symbols(buffer, nptrs);
        if (strings == NULL) {
                perror("backtrace_symbols");
                exit(EXIT_FAILURE);
        }
 
        for (j = 0; j < nptrs; j++)
                printf("  [%02d] %s\n", j, strings[j]);
 
        free(strings);
}
 
void signal_handler(int signo)
{
        
#if 0        
        char buff[64] = {0x00};
                
        sprintf(buff,"cat /proc/%d/maps", getpid());
                
        system((const char*) buff);
#endif        
 
        printf("\n=========>>>catch signal %d <<<=========\n", signo);
        
        printf("Dump stack start...\n");
        dump();
        printf("Dump stack end...\n");
        fflush(stdout);
 
        signal(signo, SIG_DFL); /* 恢复信号默认处理 */
        raise(signo);           /* 重新发送信号 */
}

void RegSig() {
    signal(SIGSEGV, signal_handler);
}

void fun3() {
    printf("this is fun3\n");
    *(char*)0 = 1; // read nullptr
}

void fun2() {
    printf("this is fun2\n");
    fun3();
}

void fun1() {
    printf("this is fun1\n");
    fun2();
}
   
 int  main() 
 { 
         RegSig();
          fun1(); 
          return  0; 
 } 