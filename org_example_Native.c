#include <stdlib.h>
#include <jni.h>
#include <curl/curl.h>

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1init
  (JNIEnv *env, jclass jcls) {
    return (jlong) curl_easy_init();
}

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1URL
  (JNIEnv *env, jclass jcls, jlong jptr, jstring jurl) {
    CURL *curl = (CURL *) jptr;
    const char *url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (url == NULL) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }
    return curl_easy_setopt(curl, CURLOPT_URL, url);
}

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1FOLLOWLOCATION
  (JNIEnv *env, jclass jcls, jlong jptr, jint jcode) {
    CURL *curl = (CURL *) jptr;
    return curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jcode);
}

JNIEXPORT void JNICALL Java_org_example_Native_curl_1easy_1cleanup
  (JNIEnv *env, jclass jcls, jlong jptr) {
    CURL *curl = (CURL *) jptr;
    curl_easy_cleanup(curl);
}

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1perform
  (JNIEnv *env, jclass jcls, jlong jptr) {
    CURL *curl = (CURL *) jptr;
    return curl_easy_perform(curl);
}

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA__JJ
  (JNIEnv *env, jclass jcls, jlong jcurl, jstring jfile) {
    CURL *curl = (CURL *) jcurl;
    FILE *fp = (FILE *) jfile;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
}

struct write_data_out {
    size_t i;
    size_t total;
    JNIEnv *env;
    jobject jout;
    jmethodID jwrite;
    jbyteArray jbuf;
};


static size_t write_callback_out(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct write_data_out * wd = (struct write_data_out *) userdata;
    JNIEnv *env = wd->env;

    jbyteArray jbuf = (*env)->NewByteArray(env, 32000); // TODO free

    jclass jOutputStream = (*env)->FindClass(env, "java/io/ByteArrayOutputStream");
    jmethodID jwrite = (*env)->GetMethodID(env, jOutputStream, "write", "([BII)V");

    printf("aaa\n");

    jmethodID constructor = (*env)->GetMethodID(env, jOutputStream, "<init>", "()V");
    /* jobject jout = (*env)->NewObject(env, jOutputStream, constructor); */

    jobject jout = wd->jout;

    printf("bbb\n");

    (*env)->SetByteArrayRegion(env, jbuf, 0, total, (jbyte *) data);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return -1;
    }

    (*env)->CallVoidMethod(env, jout, jwrite, jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return -1;
    }

    wd->i++;
    wd->total += total;
    /* printf("write_callback: %lu, %lu\n", size, nmemb); */
    /* printf("write_callback: %lu, %ld\n", wd->i, wd->total); */
    return total;
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA_1OUT
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong jwdout) {

    CURL *curl = (CURL *) jcurl;
    struct write_data_out *wd = (struct write_data_out *) jwdout;

    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_out);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, wd);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_fopen
  (JNIEnv *env, jclass jcls, jstring jpath) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    FILE *fp = fopen(path, "wb");
    return (jlong) fp;
}

JNIEXPORT void JNICALL Java_org_example_Native_fclose
  (JNIEnv *env, jclass jcls, jlong jptr) {
    FILE *fp = (FILE *) jptr;
    fclose(fp);
}

JNIEXPORT jlong JNICALL Java_org_example_Native_init_1write_1data_1out
  (JNIEnv *env, jclass jcls, jobject jout) {
    jclass jOutputStream = (*env)->FindClass(env, "java/io/OutputStream");
    jmethodID jwrite = (*env)->GetMethodID(env, jOutputStream, "write", "([BII)V");

    // TODO check
    if (jOutputStream == NULL) {
        printf("outputStreamClass is null");
    }

    if (jwrite == NULL) {
        printf("writeMethod is null");
    }

    jobject jout_blobal = (*env)->NewGlobalRef(env, jout);

    struct write_data_out *wd = malloc(sizeof(struct write_data_out));
    wd->i = 0;
    wd->total = 0;
    wd->env = env;
    wd->jout = jout_blobal;
    wd->jwrite = jwrite;
    wd->jbuf = (*env)->NewByteArray(env, 32000); // TODO free
    return (jlong) wd;
}

JNIEXPORT void JNICALL Java_org_example_Native_free
  (JNIEnv *env, jclass jcls, jlong jptr) {
    free((void *) jptr);
}
