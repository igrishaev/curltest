#include <string.h>
#include <jni.h>
#include "curl_data.h"
#include "macros.h"
#include "globals.h"
#include "debug.h"
#include "curl/curl.h"
#include "read_data.h"


size_t read_callback_stream(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct curl_data *cd = (struct curl_data *) userdata;
    JNIEnv *env = cd->env;

    size_t limit = _MIN(total, cd->size);
    debug("read callback stream, total: %lu, limit: %lu", total, limit);

    jint read = JNI_CALL(env, CallIntMethod, cd->jobj, _g.InputStream.read_BaII, cd->jbuf, 0, limit);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("InputStream.read has failed");
#ifdef DEBUG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_READFUNC_ABORT;
    }

    char * buf = JNI_CALL(env, GetPrimitiveArrayCritical, cd->jbuf, NULL);
    if (!buf) {
#ifdef DEBUG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_READFUNC_ABORT;
    }

    memcpy(ptr, buf, limit);

    JNI_CALL(env, ReleasePrimitiveArrayCritical, cd->jbuf, buf, JNI_ABORT);

    cd->i++;
    cd->total += read;

    return read;
}


size_t read_callback_file(char *ptr, size_t size, size_t nmemb, void *userdata) {
    debug("read callback file, nmemb: %lu", nmemb);
    FILE *f = (FILE *) userdata;
    size_t result = fread(ptr, size, nmemb, f);
    debug("read callback file, read: %lu", result);
    return result;
}
