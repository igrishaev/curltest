#include <stdlib.h>
#include "curl_data.h"
#include "macros.h"
#include "debug.h"
#include "globals.h"
#include "curl/curl.h"
#include "curl_data.h"

size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct curl_data *cd = (struct curl_data *) userdata;
    JNIEnv *env = cd->env;

    debug("write callback stream, total: %lu", total);

    JNI_CALL(env, SetByteArrayRegion, cd->jbuf, 0, total, (jbyte *) data);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("SetByteArrayRegion has failed");
#ifdef DEBUG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_WRITEFUNC_ERROR;
    }

    JNI_CALL(env, CallVoidMethod, cd->jobj, _g.OutputStream.write_BaII, cd->jbuf, 0, total);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("write callback stream has failed");
#ifdef DEBUG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_WRITEFUNC_ERROR;
    }
    cd->i++;
    cd->total += total;
    return total;
}

size_t write_callback_handler(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct curl_data *cd = (struct curl_data *) userdata;
    JNIEnv *env = cd->env;

    debug("write callback handler, total: %lu", total);

    JNI_CALL(env, SetByteArrayRegion, cd->jbuf, 0, total, (jbyte *) data);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("SetByteArrayRegion has failed");
#ifdef DEBIG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_WRITEFUNC_ERROR;
    }

    JNI_CALL(env, CallVoidMethod, cd->jobj, _g.IWriteHandler.handle_BaII, cd->jbuf, 0, total);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("write callback handler has failed");
#ifdef DEBIG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_WRITEFUNC_ERROR;
    }
    cd->i++;
    cd->total += total;
    return total;
}
