#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define USE_PTHREAD_CANCLESTATE
// #define USE_PTHREAD_CLEANUP

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void thread_report_sd_cleanup(void *argv) 
{
    printf("xzxxxxxxxxxxxx thread thread_report_sd_cleanup 111\n");
    pthread_mutex_t *lock = (pthread_mutex_t *)argv;
    pthread_mutex_unlock(lock);
    printf("xzxxxxxxxxxxxx thread thread_report_sd_cleanup 222\n");
}

void *thread_report_sd_property(void *argv) 
{      
    printf("xzxxxxxxxxxxxx thread sd 111\n");
#ifdef USE_PTHREAD_CANCLESTATE
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif

#ifdef USE_PTHREAD_CLEANUP
    pthread_cleanup_push(thread_report_sd_cleanup, &mutex);
#endif
    pthread_mutex_lock(&mutex);

    sleep(20);
    // pthread_exit(0);

    pthread_mutex_unlock(&mutex);

#ifdef USE_PTHREAD_CLEANUP
    pthread_cleanup_pop(0);
#endif

#ifdef USE_PTHREAD_CANCLESTATE
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif
    printf("xzxxxxxxxxxxxx thread sd 222\n");

    // sleep(5);
    printf("xzxxxxxxxxxxxx thread sd 333\n");


    return NULL;
}

int main(int argc, const char** argv) {

    pthread_t sd_report_t = 0;
    pthread_create(&sd_report_t, NULL, thread_report_sd_property, NULL);


    sleep(1);

    int iRet = pthread_cancel(sd_report_t);
    printf("xzxxxxxxxxxxxx thread sd ret:%d\n", iRet);
    pthread_join(sd_report_t, NULL);
    printf("xzxxxxxxxxxxxx thread sd done ret:%d\n", iRet);

    return 0;
}