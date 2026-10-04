#include <jni.h>
#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "curl/curl.h"

#define GET_CLASS(env, clsname, clsvar) \
    clsvar = JNI_CALL(env, FindClass, clsname); \
    if (!clsvar) { \
        debug("failed to find class: " clsname); \
        return JNI_ERR; \
    }

#define SET_FIELD(env, jcls, fname, ftype, fvar) \
    fvar = JNI_CALL(env, GetFieldID, jcls, fname, ftype); \
    if (!fvar) { \
        debug("failed to find field: " fname " " ftype); \
        return JNI_ERR; \
    }

#define GET_METHOD(env, jcls, name, sig, var) \
    var = JNI_CALL(env, GetMethodID, jcls, name, sig); \
    if (!var) { \
        debug("failed to find method: " name " " sig); \
        return JNI_ERR; \
    }

struct Globals _globals;

static int J_VERSION = JNI_VERSION_1_8;

jmethodID OS_write_BaII;
jmethodID OS_close;
jmethodID IS_read_BaII;
jmethodID IS_close;

jfieldID Request_url;
jfieldID Request_method;
jfieldID Request_followLocation;
jfieldID Request_headers;
jfieldID Request_writeFile;
jfieldID Request_writeStream;
jfieldID Request_readString;
jfieldID Request_readBytes;
jfieldID Request_accumulate;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    _globals.bar = 1;
    _globals.baz = 1;

    debug("JNI_OnLoad start");

    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, J_VERSION) != JNI_OK) {
        return JNI_ERR;
    } else {

        CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (code != CURLE_OK) {
            return JNI_ERR;
        }
        debug("cURL has been initialized globally");

        char * version = curl_version();
        debug("cURL version: %s", version);

        jclass jcls;
        jmethodID jmeth;
        jfieldID jfield;

        /* http.Request */
        GET_CLASS(env, "org/example/Request", jcls)
        SET_FIELD(env, jcls, "url",            J_STRING,     Request_url);
        SET_FIELD(env, jcls, "method",         J_INT,        Request_method);
        SET_FIELD(env, jcls, "followLocation", J_INT,        Request_followLocation);
        SET_FIELD(env, jcls, "headers",        J_STRING_ARR, Request_headers);
        SET_FIELD(env, jcls, "writeFile",      J_STRING,     Request_writeFile);
        SET_FIELD(env, jcls, "writeStream",    J_OS,         Request_writeStream);
        SET_FIELD(env, jcls, "readString",     J_STRING,     Request_readString);
        SET_FIELD(env, jcls, "readBytes",      J_BA,         Request_readBytes);
        SET_FIELD(env, jcls, "accumulate",     J_BOOL,       Request_accumulate);

        /* OutputStream */
        GET_CLASS(env, "java/io/OutputStream", jcls);
        GET_METHOD(env, jcls, "write", "([BII)V", OS_write_BaII);
        GET_METHOD(env, jcls, "close", "()V",     OS_close);

        /* InputStream */
        GET_CLASS(env, "java/io/InputStream", jcls);
        GET_METHOD(env, jcls, "read",  "([BII)I", IS_read_BaII);
        GET_METHOD(env, jcls, "close", "()V",     IS_close);

        debug("JNI_OnLoad end");
        return J_VERSION;
    }
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    debug("JNI_OnUnload");
    curl_global_cleanup();
    debug("cURL has been globally cleaned up");
}

JNIEXPORT jlong JNICALL Java_org_example_Native_get_1null
  (JNIEnv *env, jclass jcls) {
    return (jlong) NULL;
}
