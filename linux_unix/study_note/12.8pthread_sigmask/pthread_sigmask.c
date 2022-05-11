#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void sighandler(int signo)
{
    alarm(2);  
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    printf("receive signal :%d , s is :%ld\n", signo, time.tv_sec);
}

void *pthread_func(void* data)
{
    timespec time;
    alarm(2);               
    while(1) {
        clock_gettime(CLOCK_REALTIME, &time);
        printf("pthread func, s is: %ld\n", time.tv_sec);
        pause();
    }
}

int main(int argc, char **argv)
{
    int rt = 0;
    pthread_t id;
    sigset_t mask;
    timespec time;

    signal(SIGALRM, sighandler);

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);///< 信号对进程中所有线程有效，但线程可以有自己的线程屏蔽字

    rt = pthread_create(&id, NULL, pthread_func, NULL);
    if (rt < 0) {
        perror("pthread_create");
        exit(-1);
    }
    rt = pthread_sigmask(SIG_SETMASK, &mask, NULL);
    if (rt < 0) {
        perror("pthread_sigmask");
        exit(-1);
    }

    while(1) {
        clock_gettime(CLOCK_REALTIME, &time);
        printf("main thraed, s is: %ld\n", time.tv_sec);
        sleep(10);
    }
}