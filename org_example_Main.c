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
    long total;
    JNIEnv *env;
    jclass outputStreamClass;
    jmethodID writeMethod;
    jbyteArray javaByteArray;
};

static size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata)
{
    struct write_data * wd = (struct write_data *) userdata;
    JNIEnv *env = wd->env;
    (*env)->SetByteArrayRegion(env, wd->javaByteArray, 0, nmemb, (jbyte*) data);
    (*env)->CallVoidMethod(env, wd->outputStreamClass, wd->writeMethod, wd->javaByteArray, 0, nmemb);
    wd->i++;
    wd->total += nmemb;
    printf("write_callback: %ld\n", wd->total);
    return wd->total;
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

    struct write_data wd;
    wd.i = 0;
    wd.total = 0;
    wd.env = env;
    wd.outputStreamClass = outputStreamClass;
    wd.writeMethod = writeMethod;
    wd.javaByteArray = javaByteArray;

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
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wd);

    result = curl_easy_perform(curl);
    printf("curl_easy_perform %lu\n", result);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);



    /* env->DeleteLocalRef(javaByteArray); */

    return (int) http_code;
}
