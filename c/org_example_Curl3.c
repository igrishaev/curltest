#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "accum.h"
#include "debug.h"

#include "curl/curl.h"

#define SET_FIELD(env, jcls, fname, ftype, fvar) \
    fvar = (*env)->GetFieldID(env, jcls, fname, ftype); \
    if (!fvar) { \
        return JNI_ERR; \
    }

#define GET_CLASS(env, clsname, clsvar) \
    clsvar = (*env)->FindClass(env, clsname); \
    if (!clsvar) { \
        return JNI_ERR; \
    }

#define GET_METHOD(env, jcls, name, sig, var) \
    var = (*env)->GetMethodID(env, jcls, name, sig); \
    if (!var) { \
        return JNI_ERR; \
    }

#define call(env, method, ...) (*env)->method(env, ##__VA_ARGS__)

#define J_STRING     "Ljava/lang/String;"
#define J_STRING_ARR "[Ljava/lang/String;"
#define J_INT        "I"
#define J_BOOL       "Z"
#define J_OS         "Ljava/io/OutputStream;"
#define J_BA         "[B"

static jmethodID OS_write_BaII;
static jmethodID OS_close;
static jmethodID IS_read_BaII;
static jmethodID IS_close;

static jfieldID Request_url;
static jfieldID Request_method;
static jfieldID Request_followLocation;
static jfieldID Request_headers;
static jfieldID Request_writeFile;
static jfieldID Request_writeStream;
static jfieldID Request_readString;
static jfieldID Request_readBytes;
static jfieldID Request_accumulate;

static int JVM_VER = JNI_VERSION_1_8;


jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    debug("JNI_OnLoad start");

    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {

        char * version = curl_version();
        debug("cURL version: %s", version);

        jclass jcls;
        jmethodID jmeth;
        jfieldID jfield;

        /* http.Request */
        GET_CLASS(env, "org/example/http/Request", jcls)
        SET_FIELD(env, jcls, "url",            J_STRING,     Request_url);
        SET_FIELD(env, jcls, "method",         J_INT,        Request_method);
        SET_FIELD(env, jcls, "followLocation", J_INT,        Request_followLocation);
        SET_FIELD(env, jcls, "headers",        J_STRING_ARR, Request_headers);
        SET_FIELD(env, jcls, "writeFile",      J_STRING,     Request_writeFile);
        SET_FIELD(env, jcls, "writeStream",    J_OS,         Request_writeStream);
        SET_FIELD(env, jcls, "readString",     J_STRING,     Request_readString);
        SET_FIELD(env, jcls, "readBytes",      J_BA,         Request_readBytes);
        SET_FIELD(env, jcls, "accumulate",     J_BOOL,       Request_accumulate);

        /* OutputStream */
        GET_CLASS(env, "java/io/OutputStream", jcls);
        GET_METHOD(env, jcls, "write", "([BII)V", OS_write_BaII);
        GET_METHOD(env, jcls, "close", "()V",     OS_close);

        /* InputStream */
        GET_CLASS(env, "java/io/InputStream", jcls);
        GET_METHOD(env, jcls, "read",  "([BII)I", IS_read_BaII);
        GET_METHOD(env, jcls, "close", "()V",     IS_close);

        debug("JNI_OnLoad end");
        return JVM_VER;
    }
}

struct user_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject jobj;
};

struct user_data * make_user_data(JNIEnv *env, jobject jobj) {
    jbyteArray jbuf = call(env, NewByteArray, CURL_MAX_WRITE_SIZE);
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
    call(ud->env, DeleteLocalRef, ud->jbuf);
    free(ud);
}


static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    debug("write callback stream, total: %lu", total);

    call(env, SetByteArrayRegion, ud->jbuf, 0, total, (jbyte *) data);

    call(env, CallVoidMethod, ud->jobj, OS_write_BaII, ud->jbuf, 0, total);
    if (call(env, ExceptionCheck)) {
        call(env, ExceptionDescribe); // TODO: better handling
        call(env, ExceptionClear);
        return CURL_WRITEFUNC_ERROR;
    }

    ud->i++;
    ud->total += total;
    return total;
}


static long _set_url(JNIEnv *env, CURL *curl, jstring jurl) {
    int code = 0;
    char *url = NULL;

    url = call(env, GetStringUTFChars, jurl, NULL);
    if (!url) {
        code = -1; // TODO use enum
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    call(env, ReleaseStringUTFChars, jurl, url);

exit:
    return code;
}


static long _set_method(CURL *curl, jint jmethod) {
    int code = 0;
    switch (jmethod) {
        case 1: { // TODO use enumb
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

    jsize headerLen = call(env, GetArrayLength, jheaders);

    for (i = 0; i < headerLen; i++) {

        jheader = (jstring) call(env, GetObjectArrayElement, jheaders, i);
        if (jheader == NULL) {
            continue;
        }

        header = call(env, GetStringUTFChars, jheader, NULL);
        if (header) {
            curl_slist_append(headers, header);
            call(env, ReleaseStringUTFChars, jheader, header); // TODO
            call(env, DeleteLocalRef, jheader);
        } else {
            code = -3;
            goto exit;
        }
    }

exit:
    return code;
}


static size_t write_callback_accum(char *data, size_t size, size_t nmemb, void *userdata)
{
    debug("accumulator callback gets called");
    size_t len = size * nmemb;
    struct accum *acc = (struct accum *) userdata;
    if (!accum_add(acc, data, len)) return CURL_WRITEFUNC_ERROR;
    return len;
}


JNIEXPORT jlong JNICALL Java_org_example_Curl3_perform (
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

    jstring jurl          = (jstring) call(env, GetObjectField, jreq, Request_url);
    jint jmethod          = call(env, GetIntField, jreq, Request_method);
    jint jfollowLocation  = call(env, GetIntField, jreq, Request_followLocation);
    jobjectArray jheaders = (jobjectArray) call(env, GetObjectField, jreq, Request_headers);
    jstring jwriteFile    = (jstring) call(env, GetObjectField, jreq, Request_writeFile);
    jobject jwriteStream  = call(env, GetObjectField, jreq, Request_writeStream);
    jstring jreadString   = (jstring) call(env, GetObjectField, jreq, Request_readString);
    jbyteArray jreadBytes = (jbyteArray) call(env, GetObjectField, jreq, Request_readBytes);
    jboolean jaccumulate  = call(env, GetBooleanField, jreq, Request_accumulate);

    // CURL *curl = curl_easy_init();
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

        const char *path = call(env, GetStringUTFChars, jwriteFile, NULL);
        if (!path) {
            code = -5;
            goto exit;
        }

        writeFile = fopen(path, "wb");
        call(env, ReleaseStringUTFChars, jwriteFile, path);
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
        writeFunction = write_callback_accum;
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
        readString = call(env, GetStringUTFChars, jreadString, NULL);
        if (!readString) {
            code = -5;
            goto exit;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readString);
        if (code != CURLE_OK) goto exit;
    }

    /* POST FIELDS BYTES */
    if (jreadBytes) {
        readBytes = call(env, GetPrimitiveArrayCritical, jreadBytes, NULL);
        if (!readBytes) {
            code = -5;
            goto exit;
        }

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBytes);
        if (code != CURLE_OK) goto exit;

        jsize length = call(env, GetArrayLength, jreadBytes);

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
        if (code != CURLE_OK) goto exit;
    }

    /* code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); */
    /* if (code != CURLE_OK) goto exit; */

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

    // if (curl)      curl_easy_cleanup(curl);
    if (headers)   curl_slist_free_all(headers);
    if (writeFile) fclose(writeFile);
    if (ud)        clear_user_data(ud);

    if (acc)       accum_free(acc);

    /* close output stream */
    if (jwriteStream) {
        call(env, CallVoidMethod, jwriteStream, OS_close);
        if (call(env, ExceptionCheck)) {
            call(env, ExceptionDescribe); // TODO: better handling
            call(env, ExceptionClear);
        }
    }

    /* release read string */
    if (readString) {
        call(env, ReleaseStringUTFChars, jreadString, readString);
    }

    /* release read bytes */
    if (readBytes) {
        call(env, ReleasePrimitiveArrayCritical, jreadBytes, readBytes, JNI_ABORT);
    }

    return code;
}



JNIEXPORT jlong JNICALL Java_org_example_Curl3__1init
  (JNIEnv *env, jclass jcls) {
    return (jlong) curl_easy_init();
}

JNIEXPORT jlong JNICALL Java_org_example_Curl3__1free
  (JNIEnv *env, jclass jcls, jlong jcurl) {
    curl_easy_cleanup((CURL *) jcurl);
}
