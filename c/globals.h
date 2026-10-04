#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <jni.h>

extern jmethodID OS_write_BaII;
extern jmethodID OS_close;
extern jmethodID IS_read_BaII;
extern jmethodID IS_close;

extern jfieldID Request_url;
extern jfieldID Request_method;
extern jfieldID Request_followLocation;
extern jfieldID Request_headers;
extern jfieldID Request_writeFile;
extern jfieldID Request_writeStream;
extern jfieldID Request_readString;
extern jfieldID Request_readBytes;
extern jfieldID Request_accumulate;

struct Globals {
    int foo;
    int bar;
    int baz;
};

extern struct Globals _globals;

#endif /* __GLOBALS_H */
