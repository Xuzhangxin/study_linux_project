#include <string.h>
#include <stdio.h>
#include "base.h"

void MY_PRINTF(const char *fmt, ...)
{
#if defined(DEBUG_BASE_SDK) && (DEBUG_BASE_SDK == 1)
    printf(fmt);
    fflush(stdout);
#endif
}