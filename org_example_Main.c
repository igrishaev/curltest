#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <curl/curl.h>


JNIEXPORT jint JNICALL Java_org_example_Main_curlInit
  (JNIEnv *env, jclass cls)
{
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    return (int)result;
}

struct write_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmid;
    jbyteArray jarr;
};

struct curl_object {
    CURL *curl;
    char *buf;
    size_t len;
};

static void curl_append(struct curl_object * cobj, char *buf, size_t len) {
    void *tmp = realloc(cobj->buf, cobj->len + len);
    /* TODO check tmp */
    cobj->buf = tmp;
    memcpy(cobj->buf + cobj->len, buf, len);
    cobj->len += len;
}


static size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t len = size * nmemb;
    struct write_data * wd = (struct write_data *) userdata;
    JNIEnv *env = wd->env;
    (*env)->SetByteArrayRegion(env, wd->jarr, 0, len, (jbyte *) data);
    /* if ((*env)->ExceptionCheck(env)) { */
    /*     (*env)->ExceptionDescribe(env); */
    /*     (*env)->ExceptionClear(env); */
    /*     return -1; */
    /* } */
    (*env)->CallVoidMethod(env, wd->jobj, wd->jmid, wd->jarr, 0, len);
    wd->i++;
    wd->total += len;
    /* printf("write_callback: %lu, %ld\n", wd->i, wd->total); */
    return len;
}

JNIEXPORT jint JNICALL Java_org_example_Main_foobar
  (JNIEnv *env, jclass cls, jobject baos)
{
    CURL *curl;
    long http_code = 0;
    size_t result;

    jclass outputStreamClass = (*env)->FindClass(env, "java/io/OutputStream");
    jmethodID writeMethod = (*env)->GetMethodID(env, outputStreamClass, "write", "([BII)V");
    jbyteArray javaByteArray = (*env)->NewByteArray(env, 32000);

    struct write_data *wd = malloc(sizeof(struct write_data));
    wd->i = 0;
    wd->total = 0;
    wd->env = env;
    wd->jobj = baos;
    wd->jmid = writeMethod;
    wd->jarr = javaByteArray;

    if (outputStreamClass == NULL) {
        printf("outputStreamClass is null");
    }

    if (writeMethod == NULL) {
        printf("writeMethod is null");
    }

    if (javaByteArray == NULL) {
        printf("javaByteArray is null");
    }

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://habr.com/");
    /* curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/"); */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, wd);

    result = curl_easy_perform(curl);
    printf("curl_easy_perform %lu\n", result);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    (*env)->DeleteLocalRef(env, javaByteArray);

    free(wd);

    /* env->DeleteLocalRef(javaByteArray); */

    return (int) http_code;
}


static size_t wc1(char *data, size_t size, size_t nmemb, void *userdata)
{
    struct curl_object *co = (struct curl_object *) userdata;
    printf("size: %lu\n", nmemb);
    curl_append(co, data, nmemb);
    return co->len;
}

JNIEXPORT jint JNICALL Java_org_example_Main_test1 (JNIEnv *env, jclass jcls)
{
    CURL *curl = curl_easy_init();
    size_t result;

    struct curl_object *co = malloc(sizeof(struct curl_object));
    co->curl = curl;
    co->buf = NULL;
    co->len = 0;

    curl_easy_setopt(curl, CURLOPT_URL, "https://habr.com/");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wc1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, co);

    result = curl_easy_perform(curl);
    printf("curl_easy_perform %lu\n", result);

    /* curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code); */
    curl_easy_cleanup(curl);

    result = co->len;

    free(co->buf);
    free(co);

    return (int) result;
}
