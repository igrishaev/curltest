#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <jni.h>

struct J_IWriteHandler {
    jmethodID handle_BaII;
};


struct J_OutputStream {
    jmethodID write_BaII;
    jmethodID close;
};

struct J_Request {
    jfieldID url;
    jfieldID method;
    jfieldID followLocation;
    jfieldID headersPtr;
    jfieldID writeFilePtr;
    jfieldID writeStreamPtr;
    jfieldID readString;
    jfieldID readBytes;
    jfieldID accumPtr;
    jfieldID verbose;
    jfieldID writeCallbackPtr;
    jfieldID readFilePtr;
};

struct J_Response {
    jfieldID  status;
    jfieldID  headers;
    jfieldID  contentLength;
    jfieldID  effectiveUrl;
    jmethodID addHeader;
};

struct J_InputStream {
    jmethodID read_BaII;
    jmethodID close;
};

struct J_Globals {
    struct J_Response      Response;
    struct J_Request       Request;
    struct J_OutputStream  OutputStream;
    struct J_InputStream   InputStream;
    struct J_IWriteHandler IWriteHandler;
};

extern struct J_Globals _g;

#endif /* __GLOBALS_H */
