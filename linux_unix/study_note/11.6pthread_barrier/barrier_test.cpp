#include "apue.h"
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define NTHR    8 ///< number of threads
#define NUMNUM 8000000L ///< number to sort
#define TNUM (NUMNUM/NTHR) ///< number to sort per thread

long nums[NUMNUM];


int main(int argc, char **argv)
{

    // timespec nowtime = {0x0};
    // clock_gettime(CLOCK_REALTIME, &nowtime);
    // printf("sec is %ld", nowtime.tv_sec);
}