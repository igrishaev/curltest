#include <stdlib.h>
#include <jni.h>
#include <curl/curl.h>


static jmethodID meth_OS_write_BaII;
static jmethodID meth_WDF_handle_BaII;

static int _JVM_VER = JNI_VERSION_1_8;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, _JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {
        jclass jcls;

        jcls = (*env)->FindClass(env, "java/io/OutputStream");
        /* TODO check NULL */
        meth_OS_write_BaII = (*env)->GetMethodID(env, jcls, "write", "([BII)V");

        jcls = (*env)->FindClass(env, "org/example/IWriteDataFunction");
        /* TODO check NULL */
        meth_WDF_handle_BaII = (*env)->GetMethodID(env, jcls, "handle", "([BII)V");

        return _JVM_VER;
    }
}

struct write_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject joutput_stream;
};

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

static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct write_data *wd = (struct write_data *) userdata;
    JNIEnv *env = wd->env;

    /* TODO: write bytes in a cycle? */
    (*env)->SetByteArrayRegion(env, wd->jbuf, 0, total, (jbyte *) data);

    /* TODO: check exception */
    (*env)->CallVoidMethod(env, wd->joutput_stream, meth_OS_write_BaII, wd->jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return -1;
    }

    wd->i++;
    wd->total += total;
    return total;
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA_1file
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong fp) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA_1stream
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong wd_ptr) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_stream);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, wd_ptr);
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

JNIEXPORT jlong JNICALL Java_org_example_Native_init_1write_1data_1stream
  (JNIEnv *env, jclass jcls, jobject jout) {

    jbyteArray jbuf = (*env)->NewByteArray(env, CURL_MAX_WRITE_SIZE);

    struct write_data *wd = malloc(sizeof(struct write_data));
    wd->i = 0;
    wd->total = 0;
    wd->env = env;
    wd->jbuf = jbuf;
    wd->joutput_stream = (*env)->NewGlobalRef(env, jout);

    return (jlong) wd;
}

JNIEXPORT void JNICALL Java_org_example_Native_close_1write_1data_1stream
  (JNIEnv *env, jclass jcls, jlong jptr) {
    struct write_data *wd = (struct write_data *) jptr;
    (*env)->DeleteGlobalRef(env, wd->joutput_stream);
    free(wd);
}
