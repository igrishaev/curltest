#include <jni.h>
#include <stdio.h>
#include "curl/curl.h"

static jmethodID meth_OS_write_BaII;
static jmethodID meth_IS_read_BaII;

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
        jmeth = (*env)->GetMethodID(env, jcls, "write", "([BII)V");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            meth_OS_write_BaII = jmeth;
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
            meth_IS_read_BaII = jmeth;
        }

        /* OK */
        return JVM_VER;
    }
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
    int i;
    long code;

    CURL *curl = curl_easy_init();

    /* URL */
    const char *url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (!url) {
        code = -1;
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    (*env)->ReleaseStringUTFChars(env, jurl, url);
    if (code != CURLE_OK) goto exit;

    /* method */
    switch (jmethod) {
        case 1: {
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
    struct curl_slist *headers = NULL;
    if (jheaders != NULL) {
        char *header;
        jstring jheader;
        jsize headerLen = (*env)->GetArrayLength(env, jheaders);
        for (i = 0; i < headerLen; i++) {

            jheader = (jstring) (*env)->GetObjectArrayElement(env, jheaders, i);
            if (jheader == NULL) {
                continue;
            }

            header = (*env)->GetStringUTFChars(env, jheader, NULL);
            if (!header) {
                code = -3;
                goto exit;
            }

            headers = curl_slist_append(headers, header);
            (*env)->ReleaseStringUTFChars(env, jheader, header);

            (*env)->DeleteLocalRef(env, jheader);
        }
    }
    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) goto exit;


    /* WRITING */
    FILE *writeFile = stdout;
    void *writeFunction = NULL;

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

        writeFunction = fwrite;
    }

    /* WRITE STREAM */
    if (jwriteStream != NULL) {

    }

    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeFile);
    if (code != CURLE_OK) goto exit;

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    if (code != CURLE_OK) goto exit;

    /* PERFORM */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:
    if (curl)      curl_easy_cleanup(curl);
    if (headers)   curl_slist_free_all(headers);
    if (writeFile) fclose(writeFile);

    return code;
}
