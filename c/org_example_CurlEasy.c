#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "accum.h"
#include "logging.h"
#include "globals.h"
#include "macros.h"
#include "write_data.h"
#include "read_data.h"
#include "curl/curl.h"
#include "bytebuffer.h"


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1curl_1easy_1init
  (JNIEnv *env, jclass jcls) {
    return (jlong) curl_easy_init();
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1curl_1easy_1cleanup
  (JNIEnv *env, jclass jcls, jlong jcurl) {
    curl_easy_cleanup((CURL *) jcurl);
    return 0;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1url
  (JNIEnv *env, jclass jcls, jlong jcurl, jstring jurl) {

    CURLcode code;
    char *url;
    CURL *curl = (CURL *) jcurl;

    // TODO: check in java
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
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jbb) {
    CURL *curl = (CURL *) jcurl;
    char *ptr = (char *) jbb;
    log_debug("setting error buffer, curl: %lu, ptr: %lu", curl, ptr);
    CURLcode code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ptr);
    if (code != CURLE_OK) {
        log_error("curl_easy_setopt(CURLOPT_ERRORBUFFER) has failed");
    }
    return code;
}


JNIEXPORT jstring JNICALL Java_org_example_CurlEasy__1curl_1easy_1strerror
  (JNIEnv *env, jclass jcls, jlong jcode) {
    const char *str = curl_easy_strerror(jcode);
    if (str) {
        return JNI_CALL(env, NewStringUTF, str);
    } else {
        return NULL;
    }
}

JNIEXPORT jint JNICALL Java_org_example_CurlEasy__1get_1CURL_1ERROR_1SIZE
  (JNIEnv *env, jclass jcls) {
    return CURL_ERROR_SIZE;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1curl_1easy_1reset
  (JNIEnv *env, jclass jcls, jlong jcurl) {
    CURL *curl = (CURL *) jcurl;
    curl_easy_reset(curl);
    return CURLE_OK;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1curl_1set_1headers
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jheaders) {
    CURL *curl = (CURL *) jcurl;
    return curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *) jheaders);
}

static CURLcode set_write_params(CURL *curl, void *write_data, void *write_function) {
    CURLcode code;
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, write_data);
    if (code != CURLE_OK) return code;
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
    if (code != CURLE_OK) return code;
    return code;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1write_1file
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jfile) {
    CURL *curl = (CURL *) jcurl;
    return set_write_params(curl, (FILE *) jfile, fwrite);
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1write_1stream
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jstream) {
    CURL *curl = (CURL *) jcurl;
    return set_write_params(curl, (void *) jstream, write_callback_stream);
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1write_1callback
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jptr) {
    CURL *curl = (CURL *) jcurl;
    return set_write_params(curl, (void *) jptr, write_callback_handler);
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1accumulator
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jacc) {
    CURL *curl = (CURL *) jcurl;
    return set_write_params(curl, (void *) jacc, accum_write_callback);
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1post_1fields_1bytes
  (JNIEnv *env, jclass jcls, jlong jcurl, jbyteArray jbuf, jint jlen) {

    CURLcode code;
    CURL *curl = (CURL *) jcurl;
    char *buf = JNI_CALL(env, GetPrimitiveArrayCritical, jbuf, NULL);
    if (!buf) {
        code = CURLE_OUT_OF_MEMORY;
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, buf);
    JNI_CALL(env, ReleasePrimitiveArrayCritical, jbuf, buf, JNI_ABORT);
    if (code != CURLE_OK) goto exit;

    code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jlen);
    if (code != CURLE_OK) goto exit;

exit:
    return code;
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1verbose
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jvalue) {
    CURL *curl = (CURL *) jcurl;
    return curl_easy_setopt(curl, CURLOPT_VERBOSE, jvalue);
}

static CURLcode set_read_params(CURL *curl, void *read_data, void *read_function) {
    CURLcode code;
    code = curl_easy_setopt(curl, CURLOPT_READDATA, read_data);
    if (code != CURLE_OK) return code;
    code = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_function);
    if (code != CURLE_OK) return code;
    return code;
}


static CURLcode set_seek_params(CURL *curl, void *seek_data, void *seek_function) {
    CURLcode code;
    code = curl_easy_setopt(curl, CURLOPT_SEEKDATA, seek_data);
    if (code != CURLE_OK) goto exit;
    code = curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, seek_function);
    if (code != CURLE_OK) goto exit;
exit:
    return code;
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1read_1file
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jfile) {
    CURL *curl = (CURL *) jcurl;
    CURLcode code;

    code = set_read_params(curl, (FILE *) jfile, fread);
    if (code != CURLE_OK) goto exit;

    // TODO windows
    /* code = set_read_params(curl, (FILE *) jfile, read_callback_file); */
    /* if (code != CURLE_OK) goto exit; */

    /* code = set_seek_params(curl, (FILE *) jfile, read_seek_file); */
    /* if (code != CURLE_OK) goto exit; */

    /* code = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1); */
    /* if (code != CURLE_OK) goto exit; */

    // TODO: set size?
    /* code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, ????); */
    /* if (code != CURLE_OK) goto exit; */

exit:
    return code;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1read_1stream
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jstream) {

    CURL *curl = (CURL *) jcurl;
    CURLcode code;

    // TODO
    code = set_read_params(curl, (void *) jstream, read_callback_stream);
    if (code != CURLE_OK) goto exit;

    code = set_seek_params(curl, NULL, read_seek_cannot);
    if (code != CURLE_OK) goto exit;

    code = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    if (code != CURLE_OK) goto exit;

exit:
    return code;
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1set_1follow_1location
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jvalue) {
    CURL *curl = (CURL *) jcurl;
    return curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jvalue);
}


static CURLcode _get_info_as_long(jlong jcurl, jlong jbb, CURLINFO info) {
    CURL *curl = (CURL *) jcurl;
    char *bb = (char *) jbb;
    long value;
    CURLcode code = curl_easy_getinfo(curl, info, &value);
    if (code == CURLE_OK) {
        bb = put_long(bb, value);
    }
    return code;
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1get_1response_1code
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jbb) {
    return _get_info_as_long(jcurl, jbb, CURLINFO_RESPONSE_CODE);
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1get_1connect_1time_1t
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jbb) {
    return _get_info_as_long(jcurl, jbb, CURLINFO_CONNECT_TIME_T);
}


JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1get_1content_1length_1download_1t
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jbb) {
    return _get_info_as_long(jcurl, jbb, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T);
}

JNIEXPORT jlong JNICALL Java_org_example_CurlEasy__1get_1redirect_1time_1t
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jbb) {
    return _get_info_as_long(jcurl, jbb, CURLINFO_REDIRECT_TIME_T);
}


/* https://curl.se/libcurl/c/CURLINFO_RETRY_AFTER.html */
/* https://curl.se/libcurl/c/CURLINFO_SIZE_DOWNLOAD_T.html */
/* https://curl.se/libcurl/c/CURLINFO_SPEED_DOWNLOAD_T.html */
/* https://curl.se/libcurl/c/CURLINFO_SPEED_UPLOAD_T.html */
/* https://curl.se/libcurl/c/CURLINFO_USED_PROXY.html */
/* https://curl.se/libcurl/c/CURLINFO_NUM_CONNECTS.html */
/* https://curl.se/libcurl/c/CURLINFO_PRIMARY_IP.html */
/* https://curl.se/libcurl/c/CURLINFO_PRIMARY_PORT.html */
/* https://curl.se/libcurl/c/CURLINFO_PROXY_ERROR.html */
/* https://curl.se/libcurl/c/CURLINFO_REDIRECT_COUNT.html */
/* https://curl.se/libcurl/c/CURLINFO_EFFECTIVE_URL.html */
/* https://curl.se/libcurl/c/CURLINFO_HTTP_VERSION.html */
/* https://curl.se/libcurl/c/CURLINFO_LOCAL_IP.html */
/* https://curl.se/libcurl/c/CURLINFO_LOCAL_PORT.html */
/* https://curl.se/libcurl/c/CURLINFO_OS_ERRNO.html */
/* headers */

/* https://curl.se/libcurl/c/CURLOPT_AWS_SIGV4.html */
/* https://curl.se/libcurl/c/CURLOPT_BUFFERSIZE.html */
/* https://curl.se/libcurl/c/CURLOPT_CONNECTTIMEOUT_MS.html */
/* https://curl.se/libcurl/c/CURLOPT_COOKIE.html */
/* https://curl.se/libcurl/c/CURLOPT_COOKIEFILE.html */
/* https://curl.se/libcurl/c/CURLOPT_COOKIEJAR.html */
/* https://curl.se/libcurl/c/CURLOPT_COOKIESESSION.html */
/* https://curl.se/libcurl/c/CURLOPT_CUSTOMREQUEST.html */
/* https://curl.se/libcurl/c/CURLOPT_DEFAULT_PROTOCOL.html */
/* https://curl.se/libcurl/c/CURLOPT_FAILONERROR.html */
/* https://curl.se/libcurl/c/CURLOPT_HTTPHEADER.html */
/* https://curl.se/libcurl/c/CURLOPT_HTTPPROXYTUNNEL.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXY.html */
/* https://curl.se/libcurl/c/CURLOPT_HTTP_VERSION.html */
/* https://curl.se/libcurl/c/CURLOPT_LOGIN_OPTIONS.html */
/* https://curl.se/libcurl/c/CURLOPT_MAXCONNECTS.html */
/* https://curl.se/libcurl/c/CURLOPT_MAXREDIRS.html */
/* https://curl.se/libcurl/c/CURLOPT_NOBODY.html */
/* https://curl.se/libcurl/c/CURLOPT_PASSWORD.html */
/* https://curl.se/libcurl/c/CURLOPT_USERNAME.html */
/* https://curl.se/libcurl/c/CURLOPT_PRE_PROXY.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYAUTH.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYPASSWORD.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYPORT.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYTYPE.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYUSERNAME.html */
/* https://curl.se/libcurl/c/CURLOPT_PROXYUSERPWD.html */
/* https://curl.se/libcurl/c/CURLOPT_RANGE.html */
/* https://curl.se/libcurl/c/CURLOPT_SERVER_RESPONSE_TIMEOUT_MS.html */
/* https://curl.se/libcurl/c/CURLOPT_SSLCERT.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_FASTOPEN.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_KEEPALIVE.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_KEEPCNT.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_KEEPIDLE.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_KEEPINTVL.html */
/* https://curl.se/libcurl/c/CURLOPT_TCP_NODELAY.html */
/* https://curl.se/libcurl/c/CURLOPT_USERAGENT.html */
/* https://curl.se/libcurl/c/CURLOPT_TIMEOUT_MS.html */
/* https://curl.se/libcurl/c/CURLOPT_USERAGENT.html */
/* https://curl.se/libcurl/c/CURLOPT_USERNAME.html */
/* https://curl.se/libcurl/c/CURLOPT_XOAUTH2_BEARER.html */
