#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <jni.h>

struct J_OutputStream {
    jmethodID write_BaII;
    jmethodID close;
};

struct J_Request {
    jfieldID url;
    jfieldID method;
    jfieldID followLocation;
    jfieldID headers;
    jfieldID writeFile;
    jfieldID writeStream;
    jfieldID readString;
    jfieldID readBytes;
    jfieldID accumulate;
};

struct J_InputStream {
    jmethodID read_BaII;
    jmethodID close;
};

struct J_Globals {
    struct J_Request Request;
    struct J_OutputStream OutputStream;
    struct J_InputStream InputStream;
};

extern struct J_Globals _g;

#endif /* __GLOBALS_H */
