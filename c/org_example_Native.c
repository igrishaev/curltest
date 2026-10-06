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

#define SET_METHOD(env, jcls, name, sig, var) \
    var = JNI_CALL(env, GetMethodID, jcls, name, sig); \
    if (!var) { \
        debug("failed to find method: " name " " sig); \
        return JNI_ERR; \
    }

static int J_VERSION = JNI_VERSION_1_8;

struct J_Globals _g;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

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

        /* IWriteHandler */
        GET_CLASS(env, "org/example/IWriteHandler", jcls);
        SET_METHOD(env, jcls, "handle", "([BII)V", _g.IWriteHandler.handle_BaII);

        /* Response */
        GET_CLASS(env, "org/example/Response", jcls);
        SET_FIELD(env, jcls, "status",        J_INT,     _g.Response.status);
        SET_FIELD(env, jcls, "headers",       J_MAP,     _g.Response.headers);
        SET_FIELD(env, jcls, "contentLength", J_LONG,    _g.Response.contentLength);
        SET_FIELD(env, jcls, "effectiveUrl",  J_STRING,  _g.Response.effectiveUrl);
        SET_METHOD(env, jcls, "addHeader", "(" J_STRING J_STRING ")V", _g.Response.addHeader);

        /* Request */
        GET_CLASS(env, "org/example/Request", jcls)
        SET_FIELD(env, jcls, "url",              J_STRING, _g.Request.url);
        SET_FIELD(env, jcls, "method",           J_INT,    _g.Request.method);
        SET_FIELD(env, jcls, "followLocation",   J_INT,    _g.Request.followLocation);
        SET_FIELD(env, jcls, "headersPtr",       J_LONG,   _g.Request.headersPtr);
        SET_FIELD(env, jcls, "writeFilePtr",     J_LONG,   _g.Request.writeFilePtr);
        SET_FIELD(env, jcls, "writeStreamPtr",   J_LONG,   _g.Request.writeStreamPtr);
        SET_FIELD(env, jcls, "readString",       J_STRING, _g.Request.readString);
        SET_FIELD(env, jcls, "readBytes",        J_BA,     _g.Request.readBytes);
        SET_FIELD(env, jcls, "accumPtr",         J_LONG,   _g.Request.accumPtr);
        SET_FIELD(env, jcls, "verbose",          J_LONG,   _g.Request.verbose);
        SET_FIELD(env, jcls, "writeCallbackPtr", J_LONG,   _g.Request.writeCallbackPtr);

        /* OutputStream */
        GET_CLASS(env, "java/io/OutputStream", jcls);
        SET_METHOD(env, jcls, "write", "([BII)V", _g.OutputStream.write_BaII);
        SET_METHOD(env, jcls, "close", "()V",     _g.OutputStream.close);

        /* InputStream */
        GET_CLASS(env, "java/io/InputStream", jcls);
        SET_METHOD(env, jcls, "read",  "([BII)I", _g.InputStream.read_BaII);
        SET_METHOD(env, jcls, "close", "()V",     _g.InputStream.close);

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
