#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"

#define DEBUG(fmt, ...) \
    do { \
        fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

static jmethodID OS_write_BaII;
static jmethodID OS_close;
static jmethodID IS_read_BaII;

static jfieldID Request_url;
static jfieldID Request_method;
static jfieldID Request_followLocation;
static jfieldID Request_headers;
static jfieldID Request_writeFile;
static jfieldID Request_writeStream;
static jfieldID Request_readString;
static jfieldID Request_readBytes;


static int JVM_VER = JNI_VERSION_1_8;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    DEBUG("JNI_OnLoad start");

    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {

        char * version = curl_version();
        DEBUG("cURL version: %s", version);

        jclass jcls;
        jmethodID jmeth;
        jfieldID jfield;

        /* http.Request */
        jcls = (*env)->FindClass(env, "org/example/http/Request");
        if (!jcls) {
            DEBUG("Request class not found");
            return JNI_ERR;
        }

        /* url */
        jfield = (*env)->GetFieldID(env, jcls, "url", "Ljava/lang/String;");
        if (!jfield) {
            DEBUG("Request.url field not found");
            return JNI_ERR;
        }
        Request_url = jfield;

        /* method */
        jfield = (*env)->GetFieldID(env, jcls, "method", "I");
        if (!jfield) {
            DEBUG("Request.method field not found");
            return JNI_ERR;
        }
        Request_method = jfield;

        /* followLocation */
        jfield = (*env)->GetFieldID(env, jcls, "followLocation", "I");
        if (!jfield) {
            DEBUG("Request.followLocation field not found");
            return JNI_ERR;
        }
        Request_followLocation = jfield;

        /* headers */
        jfield = (*env)->GetFieldID(env, jcls, "headers", "[Ljava/lang/String;");
        if (!jfield) {
            DEBUG("Request.headers field not found");
            return JNI_ERR;
        }
        Request_headers = jfield;

        /* writeFile */
        jfield = (*env)->GetFieldID(env, jcls, "writeFile", "Ljava/lang/String;");
        if (!jfield) {
            DEBUG("Request.writeFile field not found");
            return JNI_ERR;
        }
        Request_writeFile = jfield;

        /* writeStream */
        jfield = (*env)->GetFieldID(env, jcls, "writeStream", "Ljava/io/OutputStream;");
        if (!jfield) {
            DEBUG("Request.writeStream field not found");
            return JNI_ERR;
        }
        Request_writeStream = jfield;

        /* readString */
        jfield = (*env)->GetFieldID(env, jcls, "readString", "Ljava/lang/String;");
        if (!jfield) {
            DEBUG("Request.readString field not found");
            return JNI_ERR;
        }
        Request_readString = jfield;

        /* readBytes */
        jfield = (*env)->GetFieldID(env, jcls, "readBytes", "[B");
        if (!jfield) {
            DEBUG("Request.readBytes field not found");
            return JNI_ERR;
        }
        Request_readBytes = jfield;

        /* OutputStream */
        jcls = (*env)->FindClass(env, "java/io/OutputStream");
        if (!jcls) {
            DEBUG("OutputStream class not found");
            return JNI_ERR;
        }
        /* write */
        jmeth = (*env)->GetMethodID(env, jcls, "write", "([BII)V");
        if (!jmeth) {
            DEBUG("OutputStream.write(BaII) method not found");
            return JNI_ERR;
        } else {
            OS_write_BaII = jmeth;
        }
        /* close */
        jmeth = (*env)->GetMethodID(env, jcls, "close", "()V");
        if (!jmeth) {
            DEBUG("OutputStream.close method not found");
            return JNI_ERR;
        } else {
            OS_close = jmeth;
        }

        /* InputStream */
        jcls = (*env)->FindClass(env, "java/io/InputStream");
        if (!jcls) {
            DEBUG("InputStream class not found");
            return JNI_ERR;
        }
        jmeth = (*env)->GetMethodID(env, jcls, "read", "([BII)I");
        if (!jmeth) {
            DEBUG("InputStream.read(BaII) method not found");
            return JNI_ERR;
        } else {
            IS_read_BaII = jmeth;
        }

        DEBUG("JNI_OnLoad end");

        /* OK */
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
    jbyteArray jbuf = (*env)->NewByteArray(env, CURL_MAX_WRITE_SIZE);
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
    JNIEnv *env = ud->env;
    (*env)->DeleteLocalRef(env, ud->jbuf);
    free(ud);
}


static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    DEBUG("write callback stream, total: %lu", total);

    (*env)->SetByteArrayRegion(env, ud->jbuf, 0, total, (jbyte *) data);

    (*env)->CallVoidMethod(env, ud->jobj, OS_write_BaII, ud->jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env); // TODO: better handling
        (*env)->ExceptionClear(env);
        return CURL_WRITEFUNC_ERROR;
    }

    ud->i++;
    ud->total += total;
    return total;
}


static long _set_url(JNIEnv *env, CURL *curl, jstring jurl) {
    int code = 0;
    char *url = NULL;

    url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (!url) {
        code = -1; // TODO use enum
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    (*env)->ReleaseStringUTFChars(env, jurl, url);

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

    jsize headerLen = (*env)->GetArrayLength(env, jheaders);

    for (i = 0; i < headerLen; i++) {

        jheader = (jstring) (*env)->GetObjectArrayElement(env, jheaders, i);
        if (jheader == NULL) {
            continue;
        }

        header = (*env)->GetStringUTFChars(env, jheader, NULL);
        if (header) {
            curl_slist_append(headers, header);
            (*env)->ReleaseStringUTFChars(env, jheader, header); // TODO
            (*env)->DeleteLocalRef(env, jheader);
        } else {
            code = -3;
            goto exit;
        }
    }

exit:
    return code;
}


JNIEXPORT jlong JNICALL Java_org_example_Curl3_perform (
    JNIEnv *env,
    jclass jcls,
    jobject jreq
)
{
    int i = 0;
    long code = 0;
    struct user_data * ud      = NULL;
    struct curl_slist *headers = NULL;
    void *writeData            = NULL;
    void *writeFunction        = NULL;
    FILE *writeFile            = NULL;
    char *readString           = NULL;
    void *readBytes           = NULL;

    CURL *curl = curl_easy_init();

    jstring jurl          = (jstring) (*env)->GetObjectField(env, jreq, Request_url);
    jint jmethod          = (*env)->GetIntField(env, jreq, Request_method);
    jint jfollowLocation  = (*env)->GetIntField(env, jreq, Request_followLocation);
    jobjectArray jheaders = (jobjectArray) (*env)->GetObjectField(env, jreq, Request_headers);
    jstring jwriteFile    = (jstring) (*env)->GetObjectField(env, jreq, Request_writeFile);
    jobject jwriteStream  = (*env)->GetObjectField(env, jreq, Request_writeStream);
    jstring jreadString   = (jstring) (*env)->GetObjectField(env, jreq, Request_readString);
    jbyteArray jreadBytes = (jbyteArray) (*env)->GetObjectField(env, jreq, Request_readBytes);

    /* URL */
    _set_url(env, curl, jurl);
    DEBUG("url set");

    /* method */
    _set_method(curl, jmethod);
    DEBUG("method set");

    /* FOLLOW LOCATION */
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jfollowLocation);
    if (code != CURLE_OK) goto exit;
    DEBUG("follow location set");

    /* HEADERS */
    code = _set_headers(env, jheaders, headers);
    if (code != CURLE_OK) goto exit;
    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) goto exit;
    DEBUG("headers set");

    /* WRITE FILE */
    if (jwriteFile) {

        const char *path = (*env)->GetStringUTFChars(env, jwriteFile, NULL);
        if (!path) {
            code = -5;
            goto exit;
        }

        writeFile = fopen(path, "wb");
        (*env)->ReleaseStringUTFChars(env, jwriteFile, path);
        if (!writeFile) {
            DEBUG("failed to open write file: %s", path);
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
        readString = (*env)->GetStringUTFChars(env, jreadString, NULL);
        if (!readString) {
            code = -5;
            goto exit;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readString);
        if (code != CURLE_OK) goto exit;
    }

    /* POST FIELDS BYTES */
    if (jreadBytes) {
        readBytes = (*env)->GetPrimitiveArrayCritical(env, jreadBytes, NULL);
        if (!readBytes) {
            code = -5;
            goto exit;
        }

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBytes);
        if (code != CURLE_OK) goto exit;

        jsize length = (*env)->GetArrayLength(env, jreadBytes);

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
        if (code != CURLE_OK) goto exit;
    }

    /* code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); */
    /* if (code != CURLE_OK) goto exit; */

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

    if (curl)      curl_easy_cleanup(curl);
    if (headers)   curl_slist_free_all(headers);
    if (writeFile) fclose(writeFile);
    if (ud)        clear_user_data(ud);

    /* close output stream */
    if (jwriteStream) {
        (*env)->CallVoidMethod(env, jwriteStream, OS_close);
        if ((*env)->ExceptionCheck(env)) {
            (*env)->ExceptionDescribe(env); // TODO: better handling
            (*env)->ExceptionClear(env);
        }
    }

    /* release read string */
    if (readString) {
        (*env)->ReleaseStringUTFChars(env, jreadString, readString);
    }

    /* release read bytes */
    if (readBytes) {
        (*env)->ReleasePrimitiveArrayCritical(env, jreadBytes, readBytes, JNI_ABORT);
    }

    return code;
}
