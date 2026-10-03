#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

#ifdef DEBUG
#define debug(fmt, ...) \
    do { \
        fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define debug(fmt, ...)
#endif

#endif /* __DEBUG_H */
