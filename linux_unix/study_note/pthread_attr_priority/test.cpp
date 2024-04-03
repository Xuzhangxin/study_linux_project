#include <stdio.h>
#include <pthread.h>

void *SHMThreadFun(void *arg)
{
    return NULL;
}

int main()
{
    pthread_t thread;
    int ret = 0;
    pthread_attr_t attr;
    struct sched_param param;
    int max_priority;

    // 初始化线程属性
    pthread_attr_init(&attr);

    // 获取系统最大优先级
    max_priority = sched_get_priority_max(SCHED_FIFO);
    if (max_priority == -1) {
        perror("Failed to get max priority");
        return 1;
    }

    // 设置线程调度参数
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = max_priority; // 设置线程优先级为最高优先级
    pthread_attr_setschedparam(&attr, &param);

    ret = pthread_create(&thread, &attr, SHMThreadFun, NULL);
    if (ret != 0)
    {
        return false;
    }

    return 0;
}