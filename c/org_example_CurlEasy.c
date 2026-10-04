#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "accum.h"
#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "curl/curl.h"

struct user_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject jobj;
};

struct user_data * make_user_data(JNIEnv *env, jobject jobj) {
    jbyteArray jbuf = JNI_CALL(env, NewByteArray, CURL_MAX_WRITE_SIZE);
    struct user_data * ud = malloc(sizeof(struct user_data));
    ud->i = 0;
    ud->total = 0;
    ud->env = env;
    ud->jbuf = jbuf;
    ud->jobj = jobj;
    return ud;
}

void clear_user_data(struct user_data * ud) {
    if (ud == NULL) return;
    // JNIEnv *env = ud->env; // TODO
    JNI_CALL(ud->env, DeleteLocalRef, ud->jbuf);
    free(ud);
}


static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    debug("write callback stream, total: %lu", total);

    JNI_CALL(env, SetByteArrayRegion, ud->jbuf, 0, total, (jbyte *) data);

    JNI_CALL(env, CallVoidMethod, ud->jobj, OS_write_BaII, ud->jbuf, 0, total);
    if (JNI_CALL(env, ExceptionCheck)) {
        JNI_CALL(env, ExceptionDescribe); // TODO: better handling
        JNI_CALL(env, ExceptionClear);
        return CURL_WRITEFUNC_ERROR;
    }

    ud->i++;
    ud->total += total;
    return total;
}


static long _set_url(JNIEnv *env, CURL *curl, jstring jurl) {
    int code = 0;
    char *url = NULL;

    url = JNI_CALL(env, GetStringUTFChars, jurl, NULL);
    if (!url) {
        code = -1; // TODO use enum
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    JNI_CALL(env, ReleaseStringUTFChars, jurl, url);

exit:
    return code;
}


static long _set_method(CURL *curl, jint jmethod) {
    int code = 0;
    switch (jmethod) {
        case 1: { // TODO use enum
            code = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
            if (code != CURLE_OK) goto exit;
            break;
        }
        case 2: {
            code = curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
            if (code != CURLE_OK) goto exit;
            break;
        }
        default: {
            code = -2;
            goto exit;
        }
    }
exit:
    return code;
}

static long _set_headers(JNIEnv *env, jobjectArray jheaders, struct curl_slist *headers) {
    int i = 0;
    long code = 0;
    char *header = NULL;
    jstring jheader = NULL;

    if (jheaders == NULL) return 0;

    jsize headerLen = JNI_CALL(env, GetArrayLength, jheaders);

    for (i = 0; i < headerLen; i++) {

        jheader = (jstring) JNI_CALL(env, GetObjectArrayElement, jheaders, i);
        if (jheader == NULL) {
            continue;
        }

        header = JNI_CALL(env, GetStringUTFChars, jheader, NULL);
        if (header) {
            curl_slist_append(headers, header);
            JNI_CALL(env, ReleaseStringUTFChars, jheader, header); // TODO
            JNI_CALL(env, DeleteLocalRef, jheader);
        } else {
            code = -3; // TODO return
            goto exit;
        }
    }

exit:
    return code;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy_curl_1easy_1perform (
    JNIEnv *env,
    jclass jcls,
    jlong jcurl,
    jobject jreq
)
{
vars:
    int i = 0;
    long code = 0;
    struct user_data * ud      = NULL;
    struct curl_slist *headers = NULL;
    void *writeData            = NULL;
    void *writeFunction        = NULL;
    FILE *writeFile            = NULL;
    char *readString           = NULL;
    void *readBytes            = NULL;
    struct accum * acc         = NULL;

    jstring jurl          = (jstring) JNI_CALL(env, GetObjectField, jreq, Request_url);
    jint jmethod          = JNI_CALL(env, GetIntField, jreq, Request_method);
    jint jfollowLocation  = JNI_CALL(env, GetIntField, jreq, Request_followLocation);
    jobjectArray jheaders = (jobjectArray) JNI_CALL(env, GetObjectField, jreq, Request_headers);
    jstring jwriteFile    = (jstring) JNI_CALL(env, GetObjectField, jreq, Request_writeFile);
    jobject jwriteStream  = JNI_CALL(env, GetObjectField, jreq, Request_writeStream);
    jstring jreadString   = (jstring) JNI_CALL(env, GetObjectField, jreq, Request_readString);
    jbyteArray jreadBytes = (jbyteArray) JNI_CALL(env, GetObjectField, jreq, Request_readBytes);
    jboolean jaccumulate  = JNI_CALL(env, GetBooleanField, jreq, Request_accumulate);

    CURL *curl = (CURL *) jcurl;

    /* URL */
    _set_url(env, curl, jurl);
    debug("url set");

    /* method */
    _set_method(curl, jmethod);
    debug("method set");

    /* FOLLOW LOCATION */
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jfollowLocation);
    if (code != CURLE_OK) goto exit;
    debug("follow location set");

    /* HEADERS */
    code = _set_headers(env, jheaders, headers);
    if (code != CURLE_OK) goto exit;
    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) goto exit;
    debug("headers set");

    /* WRITE FILE */
    if (jwriteFile) {

        const char *path = JNI_CALL(env, GetStringUTFChars, jwriteFile, NULL);
        if (!path) {
            code = -5;
            goto exit;
        }

        writeFile = fopen(path, "wb");
        JNI_CALL(env, ReleaseStringUTFChars, jwriteFile, path);
        if (!writeFile) {
            debug("failed to open write file: %s", path);
            code = -4;
            goto exit;
        }

        writeData = writeFile;
        writeFunction = fwrite;
    }

    /* WRITE STREAM */
    if (jwriteStream) {
        // TODO: allocate on stack?
        ud = make_user_data(env, jwriteStream);
        writeData = ud;
        writeFunction = write_callback_stream;
        // TODO: close stream?
    }

    /* accumulate in memory */
    if (jaccumulate) {
        acc = accum_init(2024, 2);
        if (!acc) {
            debug("failed to init accumulator");
            code = -99;
            goto exit;
        }
        debug("accumulator is OK");
        writeData = acc;
        writeFunction = accum_write_callback;
    }

    /* WRITING  */
    if (writeData) {
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeData);
        if (code != CURLE_OK) goto exit;
    }
    if (writeFunction) {
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        if (code != CURLE_OK) goto exit;
    }

    /* POST FIELDS STRING */
    if (jreadString) {
        readString = JNI_CALL(env, GetStringUTFChars, jreadString, NULL);
        if (!readString) {
            code = -5;
            goto exit;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readString);
        if (code != CURLE_OK) goto exit;
    }

    /* POST FIELDS BYTES */
    if (jreadBytes) {
        readBytes = JNI_CALL(env, GetPrimitiveArrayCritical, jreadBytes, NULL);
        if (!readBytes) {
            code = -5;
            goto exit;
        }

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBytes);
        if (code != CURLE_OK) goto exit;

        jsize length = JNI_CALL(env, GetArrayLength, jreadBytes);

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
        if (code != CURLE_OK) goto exit;
    }

    /* code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); */
    /* if (code != CURLE_OK) goto exit; */

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

    if (headers)   curl_slist_free_all(headers);
    if (writeFile) fclose(writeFile);
    if (ud)        clear_user_data(ud);

    if (acc)       accum_free(acc);

    /* close output stream */
    if (jwriteStream) {
        JNI_CALL(env, CallVoidMethod, jwriteStream, OS_close);
        if (JNI_CALL(env, ExceptionCheck)) {
            JNI_CALL(env, ExceptionDescribe); // TODO: better handling
            JNI_CALL(env, ExceptionClear);
        }
    }

    /* release read string */
    if (readString) {
        JNI_CALL(env, ReleaseStringUTFChars, jreadString, readString);
    }

    /* release read bytes */
    if (readBytes) {
        JNI_CALL(env, ReleasePrimitiveArrayCritical, jreadBytes, readBytes, JNI_ABORT);
    }

    return code;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy_curl_1easy_1init
  (JNIEnv *env, jclass jcls) {
    return (jlong) curl_easy_init();
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy_curl_1easy_1cleanup
  (JNIEnv *env, jclass jcls, jlong jcurl) {
    curl_easy_cleanup((CURL *) jcurl);
    return 0;
}
