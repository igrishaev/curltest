#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <jni.h>

struct J_OutputStream {
    jclass    class;
    jmethodID write_BaII;
    jmethodID close;
};

struct J_Request {
    jclass   class;
    jfieldID url;
    jfieldID method;
    jfieldID followLocation;
    jfieldID headersPtr;
    jfieldID writeFilePtr;
    jfieldID writeStream;
    jfieldID readString;
    jfieldID readBytes;
    jfieldID accumPtr;
};

struct J_Response {
    jclass    class;
    jfieldID  status;
    jfieldID  headers;
    jfieldID  body;
    jfieldID  contentLength;
    jfieldID  effectiveUrl;
    jmethodID addHeader;
};

struct J_InputStream {
    jclass    class;
    jmethodID read_BaII;
    jmethodID close;
};

struct J_Globals {
    struct J_Response Response;
    struct J_Request Request;
    struct J_OutputStream OutputStream;
    struct J_InputStream InputStream;
};

extern struct J_Globals _g;

#endif /* __GLOBALS_H */
