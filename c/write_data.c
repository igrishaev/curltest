#include <stdlib.h>
#include "write_data.h"
#include "macros.h"
#include "debug.h"
#include "globals.h"
#include "curl/curl.h"

struct write_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject jobj;
};

struct write_data * write_data_init(JNIEnv *env, jobject jobj)
{
    jobject jbufG = NULL;
    jobject jobjG = NULL;
    jbyteArray jbuf = NULL;
    struct write_data * wd = NULL;

    jbuf = JNI_CALL(env, NewByteArray, CURL_MAX_WRITE_SIZE);
    jbufG = JNI_CALL(env, NewGlobalRef, jbuf);
    if (!jbufG) {
        debug("NewGlobalRef(jbuf) has failed");
        goto err;
    }

    JNI_CALL(env, DeleteLocalRef, jbuf);

    jobjG = JNI_CALL(env, NewGlobalRef, jobj);
    if (!jobjG) {
        debug("NewGlobalRef(jobj) has failed");
        goto err;
    }

    wd = malloc(sizeof(struct write_data));
    if (!wd) {
        debug("failed to allocate struct write_data");
        goto err;
    }

    wd->i     = 0;
    wd->total = 0;
    wd->env   = env;
    wd->jbuf  = jbufG;
    wd->jobj  = jobjG;
    return wd;

err:
    if (jbufG) JNI_CALL(env, DeleteGlobalRef, jbufG);
    if (jobjG) JNI_CALL(env, DeleteGlobalRef, jobjG);
    if (wd)    free(wd);
    return NULL;
}

void write_data_free(struct write_data * wd)
{
    if (wd == NULL) return;
    JNI_CALL(wd->env, DeleteLocalRef, wd->jbuf);
    JNI_CALL(wd->env, DeleteGlobalRef, wd->jobj);
    free(wd);
}

size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct write_data *wd = (struct write_data *) userdata;
    JNIEnv *env = wd->env;

    debug("write callback stream, total: %lu", total);

    JNI_CALL(env, SetByteArrayRegion, wd->jbuf, 0, total, (jbyte *) data);

    JNI_CALL(env, CallVoidMethod, wd->jobj, _g.OutputStream.write_BaII, wd->jbuf, 0, total);
    if (JNI_CALL(env, ExceptionCheck)) {
        return CURL_WRITEFUNC_ERROR;
    }
    wd->i++;
    wd->total += total;
    return total;
}
