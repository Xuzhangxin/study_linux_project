#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define DEBUG_BASE_SDK 0

void MY_PRINTF(const char *fmt, ...);

#define CHECK_NULL_RETURN_NULL(x)\
if (NULL == (x)) { \
    printf("%s is null!\n", #x); \
    return NULL; \
}


#define CHECK_NULL_RETURN(x)\
if (NULL == (x)) { \
    printf("%s is null!\n", #x); \
    return; \
}

#define CHECK_NULL_RETURN_VAL(x)\
if (NULL == (x)) { \
    printf("%s is null!\n", #x); \
    return -1; \
}




#endif