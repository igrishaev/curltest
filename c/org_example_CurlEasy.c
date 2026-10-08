#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "accum.h"
#include "logging.h"
#include "globals.h"
#include "macros.h"
#include "write_data.h"
#include "read_data.h"
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
    void *writeData            = NULL;
    void *writeFunction        = NULL;
    void *readData             = NULL;
    void *readFunction         = NULL;

    jstring    jurl              = (jstring) JNI_CALL(env, GetObjectField, jreq, _g.Request.url);
    jint       jmethod           = JNI_CALL(env, GetIntField, jreq, _g.Request.method);
    jint       followLocation    = JNI_CALL(env, GetIntField, jreq, _g.Request.followLocation);
    void      *headersPtr        = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.headersPtr);
    void      *writeFilePtr      = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.writeFilePtr);
    void      *writeStreamPtr    = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.writeStreamPtr);
    jstring    jreadString       = (jstring) JNI_CALL(env, GetObjectField, jreq, _g.Request.readString);
    jbyteArray jreadBytes        = (jbyteArray) JNI_CALL(env, GetObjectField, jreq, _g.Request.readBytes);
    void      *accumPtr          = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.accumPtr);
    jlong      jverbose          = JNI_CALL(env, GetLongField, jreq, _g.Request.verbose);
    void      *writeCallbackPtr  = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.writeCallbackPtr);
    void      *readFilePtr       = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.readFilePtr);
    void      *readStreamPtr     = (void *) JNI_CALL(env, GetLongField, jreq, _g.Request.readStreamPtr);

    // TODO: reuse
    CURL *curl = (CURL *) jcurl;

    // TODO: pass a flag
    curl_easy_reset(curl);
    log_debug("curl has been reset");

    /* url */
    _set_url(env, curl, jurl);
    log_debug("url set");

    /* method */
    _set_method(curl, jmethod);
    log_debug("method set");

    /* follow location */
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, followLocation);
    if (code != CURLE_OK) goto exit;
    log_debug("follow location set");

    /* headers */
    if (headersPtr) {
        code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headersPtr);
        if (code != CURLE_OK) goto exit;
        log_debug("headers set");
    }

    /* write file */
    if (writeFilePtr) {
        writeData = writeFilePtr;
        writeFunction = fwrite;
        log_debug("write file is set");
    }

    /* write stream */
    if (writeStreamPtr) {
        writeData = writeStreamPtr;
        writeFunction = write_callback_stream;
        log_debug("write stream is set");
    }

    /* write callback */
    if (writeCallbackPtr) {
        writeData = writeCallbackPtr;
        writeFunction = write_callback_handler;
        log_debug("write callback is set");
    }

    /* accumulate in memory */
    if (accumPtr) {
        log_debug("accumulator is passed");
        writeData = accumPtr;
        writeFunction = accum_write_callback;
        log_debug("accumulator is set");
    }

    /* writing  */
    if (writeData) {
        log_debug("setting write data...");
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeData);
        if (code != CURLE_OK) goto exit;
    }
    if (writeFunction) {
        log_debug("setting write function...");
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        if (code != CURLE_OK) goto exit;
    }

    /* read file */
    if (readFilePtr) {
        log_debug("read file ptr: %lu", (long) readFilePtr);
        readData = readFilePtr;

        readFunction = read_callback_file;
        /* readFunction = fread; */

        // TODO: only for windows
        code = curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, read_seek_file);
        if (code != CURLE_OK) goto exit;
        log_debug("seek function is set");

        code = curl_easy_setopt(curl, CURLOPT_SEEKDATA, readFilePtr);
        if (code != CURLE_OK) goto exit;
        log_debug("seek data is set");

        // TODO: set size?
        /* code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 1248); */
        /* if (code != CURLE_OK) goto exit; */

        code = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        if (code != CURLE_OK) goto exit;
        log_debug("UPLOAD is set");

        log_debug("read file is set");
    }

    /* read stream */
    if (readStreamPtr) {

        code = curl_easy_setopt(curl, CURLOPT_SEEKDATA, NULL);
        if (code != CURLE_OK) goto exit;
        log_debug("seek data is set");

        code = curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, read_seek_cannot);
        if (code != CURLE_OK) goto exit;
        log_debug("seek function is set");

        code = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        if (code != CURLE_OK) goto exit;
        log_debug("UPLOAD is set");

        readData = readStreamPtr;
        readFunction = read_callback_stream;
        log_debug("read stream is set");


    }

    /* reading */
    if (readData) {
        code = curl_easy_setopt(curl, CURLOPT_READDATA, readData);
        if (code != CURLE_OK) goto exit;
        log_debug("read data is set");
    }
    if (readFunction) {
        code = curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFunction);
        if (code != CURLE_OK) goto exit;
        log_debug("read function is set");
    }

    /* post string */
    if (jreadString) {
        log_debug("setting read string... ");
        char * readString = JNI_CALL(env, GetStringUTFChars, jreadString, NULL);
        if (!readString) {
            code = -5; // TODO special error code
            goto exit;
        }
        code = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, readString);
        JNI_CALL(env, ReleaseStringUTFChars, jreadString, readString);
        if (code != CURLE_OK) goto exit;
    }

    /* post bytes */
    if (jreadBytes) {
        log_debug("setting read bytes... ");
        char * readBytes = JNI_CALL(env, GetPrimitiveArrayCritical, jreadBytes, NULL);
        if (!readBytes) {
            code = -5; // TODO error const
            goto exit;
        }

        code = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, readBytes);
        JNI_CALL(env, ReleasePrimitiveArrayCritical, jreadBytes, readBytes, JNI_ABORT);
        if (code != CURLE_OK) goto exit;

        jsize length = JNI_CALL(env, GetArrayLength, jreadBytes);
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
        if (code != CURLE_OK) goto exit;
    }

    /* verbose */
    if (jverbose != 0) {
        log_debug("setting verbose flag: %ld", jverbose);
        code = curl_easy_setopt(curl, CURLOPT_VERBOSE, jverbose);
        if (code != CURLE_OK) goto exit;
    }

    /* perform */
    log_debug("running perform... ");
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

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

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1url
  (JNIEnv *env, jclass jcls, jlong jcurl, jstring jurl) {

    CURLcode code;
    char *url;
    CURL *curl = (CURL *) jcurl;

    if (!jurl) {
        log_debug("HTTP URL is NULL, setting NULL");
        code = curl_easy_setopt(curl, CURLOPT_URL, NULL);
        if (code != CURLE_OK) {
            log_error("curl_easy_setopt(CURLOPT_URL, NULL) has failed");
        }
        goto exit;
    }

    url = JNI_CALL(env, GetStringUTFChars, jurl, NULL);
    if (!url) {
        code = CURLE_OUT_OF_MEMORY;
        log_error("JNI GetStringUTFChars() has failed");
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    JNI_CALL(env, ReleaseStringUTFChars, jurl, url);
    if (code != CURLE_OK) {
        log_error("curl_easy_setopt() has failed, url: %s", url);
        goto exit;
    }

exit:
    return code;
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1method
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jmethod) {

    CURLcode code;
    CURL *curl = (CURL *) jcurl;

    switch (jmethod) {
        case 1: { // TODO use enum
            code = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
            if (code != CURLE_OK) {
                log_error("curl_easy_setopt() CURLOPT_HTTPGET has failed");
                goto exit;
            }
            break;
        }
        case 2: {
            code = curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
            if (code != CURLE_OK) {
                log_error("curl_easy_setopt() CURLOPT_HTTPPOST has failed");
                goto exit;
            }
            break;
        }
        default: {
            log_error("unknown HTTP method: %d", jmethod);
            code = CURLE_UNKNOWN_OPTION;
            goto exit;
        }
    }

exit:
    return code;

}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1perform
  (JNIEnv *env, jclass jcls, jlong jcurl) {
    CURL *curl = (CURL *) jcurl;
    return curl_easy_perform(curl);
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1error_1buffer
  (JNIEnv *env, jclass jcls, jlong jcurl, jobject jbb) {

    CURLcode code;
    char *ptr;
    CURL *curl = (CURL *) jcurl;

    ptr = JNI_CALL(env, GetDirectBufferAddress, jbb);
    if (!ptr) {
        code = CURLE_OUT_OF_MEMORY;
        log_error("JNI GetDirectBufferAddress() has failed");
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ptr);
    if (code != CURLE_OK) {
        log_error("curl_easy_setopt(CURLOPT_ERRORBUFFER) has failed");
        goto exit;
    }

exit:
    return code;
}
