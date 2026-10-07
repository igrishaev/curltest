#include <jni.h>
#include "curl_data.h"
#include "macros.h"
#include "debug.h"
#include "curl/curl.h"
#include "read_data.h"


static size_t read_callback_stream(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct curl_data *cd = (struct curl_data *) userdata;
    JNIEnv *env = cd->env;

    debug("read callback stream, total: %lu", total);

    // TOOD: store buf size
    // TODO: get min size (total, cd->buf)

    int limit;

    jint read = JNI_CALL(env, CallIntMethod, cd->jobj, _g.InputStream.read_BaII, cd->jbuf, 0, limit);
    if (JNI_CALL(env, ExceptionCheck)) {
        debug("InputStream.read has failed");
#ifdef DEBUG
        JNI_CALL(env, ExceptionDescribe);
#endif
        return CURL_READFUNC_ABORT;
    }

    char * readBytes = JNI_CALL(env, GetPrimitiveArrayCritical, jreadBytes, NULL);
    if (!readBytes) {
        code = -5; // TODO error const
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, readBytes);
    JNI_CALL(env, ReleasePrimitiveArrayCritical, jreadBytes, readBytes, JNI_ABORT);


    memcpy(ptr, buf, limit);


    cd->i++;
    cd->total += read;

    return read;
}
