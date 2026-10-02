#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"


static jmethodID OS_write_BaII;
static jmethodID OS_close;
static jmethodID IS_read_BaII;

static int JVM_VER = JNI_VERSION_1_8;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {

        char * version = curl_version();
        printf("curl version: %s \n", version);

        jclass jcls;
        jmethodID jmeth;

        /* OutputStream */
        jcls = (*env)->FindClass(env, "java/io/OutputStream");
        if (jcls == NULL) {
            return JNI_ERR;
        }
        /* write */
        jmeth = (*env)->GetMethodID(env, jcls, "write", "([BII)V");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            OS_write_BaII = jmeth;
        }
        /* close */
        jmeth = (*env)->GetMethodID(env, jcls, "close", "()V");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            OS_close = jmeth;
        }

        /* InputStream */
        jcls = (*env)->FindClass(env, "java/io/InputStream");
        if (jcls == NULL) {
            return JNI_ERR;
        }
        jmeth = (*env)->GetMethodID(env, jcls, "read", "([BII)I");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            IS_read_BaII = jmeth;
        }

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
    free(ud);
}


static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    printf("callback: %lu \n", total);

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
    jstring jurl,
    jint jmethod,
    jint jfollowLocation,
    jobjectArray jheaders,
    jstring jwriteFile,
    jobject jwriteStream
)
{
    int i = 0;
    long code = 0;
    struct user_data * ud      = NULL;
    struct curl_slist *headers = NULL;
    const char *url            = NULL;
    void *writeData            = NULL;
    void *writeFunction        = NULL;
    FILE *writeFile            = NULL;

    CURL *curl = curl_easy_init();

    /* URL */
    url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (!url) {
        code = -1; // TODO use enumb
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    (*env)->ReleaseStringUTFChars(env, jurl, url);
    if (code != CURLE_OK) goto exit;

    /* method */
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

    /* FOLLOW LOCATION */
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jfollowLocation);
    if (code != CURLE_OK) goto exit;

    /* HEADERS */
    code = _set_headers(env, jheaders, headers);
    if (code != CURLE_OK) goto exit;
    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) goto exit;

    /* WRITE FILE */
    if (jwriteFile != NULL) {

        const char *path = (*env)->GetStringUTFChars(env, jwriteFile, NULL);
        if (!path) {
            code = -5;
            goto exit;
        }

        writeFile = fopen(path, "wb");
        (*env)->ReleaseStringUTFChars(env, jwriteFile, path);
        if (!writeFile) {
            code = -4;
            goto exit;
        }

        writeData = writeFile;
        writeFunction = fwrite;
    }

    /* WRITE STREAM */
    if (jwriteStream != NULL) {

        printf("jwriteStream\n");
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

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

    if (curl)      curl_easy_cleanup(curl);
    if (headers)   curl_slist_free_all(headers);
    if (writeFile) fclose(writeFile);
    if (ud)        clear_user_data(ud);

    /* close output stream */
    if (jwriteStream != NULL) {
        (*env)->CallVoidMethod(env, jwriteStream, OS_close);
        if ((*env)->ExceptionCheck(env)) {
            (*env)->ExceptionDescribe(env); // TODO: better handling
            (*env)->ExceptionClear(env);
        }
    }

    return code;
}
