#include "apue.h"
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    int err;
    pthread_t tid;
    pthread_attr_t  attr;

    err = pthread_attr_init(&attr);
    if (err != 0) {
        return err;
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0) {
        err = pthread_create(&tid, &attr, NULL, NULL);
    }
    pthread_attr_destroy(&attr);

    return 0;
}