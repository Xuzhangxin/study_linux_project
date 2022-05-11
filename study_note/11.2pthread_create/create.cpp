#include "apue.h"
#include <pthread.h>

pthread_t ntid;

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

void *thr_fun(void *arg)
{
    printids("new thread: ");
    return 0;
}

int main(void) 
{
    int err;

    err = pthread_create(&ntid, NULL, thr_fun, NULL);
    if (err != 0) {
        err_exit(err, "error: create pthread failed");
    }
    printids("main thread:");
    sleep(1);   //直接退出，可能新线程还没创建完成
    exit(0);

}




