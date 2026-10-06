#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "accum.h"
#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "write_data.h"
#include "curl/curl.h"


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
    char *readString           = NULL;
    void *readBytes            = NULL;

    jstring    jurl              = (jstring) JNI_CALL(env, GetObjectField, jreq, _g.Request.url);
    jint       jmethod           = JNI_CALL(env, GetIntField, jreq, _g.Request.method);
    jint       jfollowLocation   = JNI_CALL(env, GetIntField, jreq, _g.Request.followLocation);
    jlong      jheadersPtr       = JNI_CALL(env, GetLongField, jreq, _g.Request.headersPtr);
    jlong      jwriteFilePtr     = JNI_CALL(env, GetLongField, jreq, _g.Request.writeFilePtr);
    jlong      jwriteStreamPtr   = JNI_CALL(env, GetLongField, jreq, _g.Request.writeStreamPtr);
    jstring    jreadString       = (jstring) JNI_CALL(env, GetObjectField, jreq, _g.Request.readString);
    jbyteArray jreadBytes        = (jbyteArray) JNI_CALL(env, GetObjectField, jreq, _g.Request.readBytes);
    jlong      jaccumPtr         = JNI_CALL(env, GetLongField, jreq, _g.Request.accumPtr);
    jlong      jverbose          = JNI_CALL(env, GetLongField, jreq, _g.Request.verbose);
    // TODO: coerce to poiners ^
    void *writeCallbackPtr       = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.writeCallbackPtr);

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
    if (jheadersPtr != NULL) {
        code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, jheadersPtr);
        if (code != CURLE_OK) goto exit;
        debug("headers set");
    }

    /* WRITE FILE */
    if (jwriteFilePtr != NULL) {
        writeData = (void *) jwriteFilePtr;
        writeFunction = fwrite;
        debug("write file is set");
    }

    /* WRITE STREAM */
    if (jwriteStreamPtr != NULL) {
        writeData = (void *) jwriteStreamPtr;
        writeFunction = write_callback_stream;
        debug("write stream is set");
    }

    /* write callback */
    if (writeCallbackPtr != NULL) {
        writeData = writeCallbackPtr;
        writeFunction = write_callback_handler;
    }

    /* accumulate in memory */
    if (jaccumPtr != NULL) {
        debug("accumulator is passed");
        writeData = (void *) jaccumPtr;
        writeFunction = accum_write_callback;
        debug("accumulator is set");
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
            code = -5; // TODO: COPY
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

        // TODO: COPY
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBytes);
        if (code != CURLE_OK) goto exit;

        jsize length = JNI_CALL(env, GetArrayLength, jreadBytes);

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
        if (code != CURLE_OK) goto exit;
    }

    /* verbose */
    if (jverbose != 0) {
        debug("setting verbose flag: %ld", jverbose);
        code = curl_easy_setopt(curl, CURLOPT_VERBOSE, jverbose);
        if (code != CURLE_OK) goto exit;
    }

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:
    // TODO
    // read string
    // read bytes
    // read string: copy!!!
    // read byte: copy!!!

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
