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


#define DEBUG_BASE_SDK 1

#define CHECK_NULL_RETURN_NULL(x)\
if (NULL == (x)) { \
    printf("%s is null!\n", #x); \
    return NULL; \
}

void MY_PRINTF(const char *fmt, ...)
{
#if defined(DEBUG_BASE_SDK) && (DEBUG_BASE_SDK == 1)
    printf(fmt);
    fflush(stdout);
#endif
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