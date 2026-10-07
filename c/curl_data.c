#include <stdlib.h>
#include <jni.h>
#include "macros.h"
#include "debug.h"
#include "curl/curl.h"
#include "curl_data.h"

struct curl_data * curl_data_init(JNIEnv *env, jobject jobj)
{
    jobject jbufG = NULL;
    jobject jobjG = NULL;
    jbyteArray jbuf = NULL;
    struct curl_data * cd = NULL;

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

    cd = malloc(sizeof(struct curl_data));
    if (!cd) {
        debug("failed to allocate struct curl_data");
        goto err;
    }

    cd->i     = 0;
    cd->total = 0;
    cd->env   = env;
    cd->jbuf  = jbufG;
    cd->jobj  = jobjG;
    return cd;

err:
    if (jbufG) JNI_CALL(env, DeleteGlobalRef, jbufG);
    if (jobjG) JNI_CALL(env, DeleteGlobalRef, jobjG);
    if (cd)    free(cd);
    return NULL;
}

void curl_data_free(struct curl_data * cd)
{
    if (cd == NULL) return;
    JNI_CALL(cd->env, DeleteLocalRef, cd->jbuf);
    JNI_CALL(cd->env, DeleteGlobalRef, cd->jobj);
    free(cd);
}
