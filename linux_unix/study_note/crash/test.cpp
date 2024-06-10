#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>


void sigHandleFunc(int signo)
{
    printf("sigHandleFunc recv sig:%d", signo);
    int j, nptrs;

    void *buffer[100];
    char **strings;
 
    nptrs = backtrace(buffer, 100);
    printf("backtrace() returned %d addresses\n", nptrs);
 
    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */
 
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
 
    for (j = 0; j < nptrs; j++){
        printf("%s\n", strings[j]);
    }
 
    free(strings);
    signal(signo,SIG_DFL);
}

void normalFunc()
{
    return ;
}

void crashFunc()
{
    char *str = NULL;   
    strcpy(str, "hello world");
    printf("str is %s\n", str);

    return ;
}

int main(int argc, char **argv)
{
    int i = 0;
    signal(SIGSEGV, sigHandleFunc); //segmentation violation
    signal(SIGABRT, sigHandleFunc); //abort program (formerly SIGIOT)
    signal(SIGPIPE, sigHandleFunc); //floating-point exception
    while (1)
    {
        if (i++ == 10)
        {
            printf("crashFunc begin\n");
            crashFunc();
            printf("crashFunc done\n");
        }
        else
        {
            printf("normalFunc begin\n");
            normalFunc();
            printf("normalFunc done\n");
        }

    }
    return 0;
}