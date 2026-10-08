#ifndef __LOGGING_H
#define __LOGGING_H

#include <stdio.h>

#define __LOG(level, fmt, ...) \
    do { \
        fprintf(stderr, "[" level "] %s:%d:%s(): " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

#define log_error(fmt, ...) __LOG("ERROR", fmt, ##__VA_ARGS__)

#ifdef DEBUG
#define log_debug(fmt, ...) __LOG("DEBUG", fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

#endif /* __LOGGING_H */
