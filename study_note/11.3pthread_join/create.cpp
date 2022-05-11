#include "apue.h"
#include <pthread.h>

int arg = 2;
void printids(const char *name)
{
    pid_t pid;
    pthread_t thread_id;
    int equal = 0;

    pid = getpid();
    thread_id = pthread_self();
    equal = pthread_equal(thread_id, pthread_self());
    if (equal) { 
        printf("equal : %d\n", equal); 
    } else { 
        printf("not equal\n"); 
    }

    printf("%s pid is %u, thread_id is %lu\n", name, pid, thread_id);

}

void *thr_fun1(void *arg)
{
    int *args = (int *)arg;
    printf("arg is %d\n", *args);
    printf("thread 1 returning\n");
    sleep(3);
    return ((void *)1);
}

void *thr_fun2(void *arg)
{
    printf("thread 2 returning\n");
    sleep(1);
    return ((void *)2);
}

int main(void) 
{
    int err;
    pthread_t tid1, tid2;
    void *tret;

    err = pthread_create(&tid1, NULL, thr_fun1, &arg);
    if (err != 0) {
        err_exit(err, "error: create pthread failed");
    }
    err = pthread_create(&tid2, NULL, thr_fun2, NULL);
    if (err != 0) {
        err_exit(err, "error: create pthread2 failed");
    }

    err = pthread_join(tid1, &tret);
    if (err != 0) {
        err_exit(err, "error: create join with thread1");
    }
    printf("thread 1 exit code %ld\n", (long)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0) {
        err_exit(err, "error: create join with thread2");
    }    
    printf("thread 2 exit code %ld\n", (long)tret);

    exit(0);

}




