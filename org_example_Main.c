#include <jni.h>
#include <curl/curl.h>


JNIEXPORT jint JNICALL Java_org_example_Main_curlInit
  (JNIEnv *env, jclass cls)
{
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    return (int)result;
}


/* static size_t cb(char *data, size_t size, size_t nmemb, void *bb) */
/* { */
/*     memcpy(bb, data, nmemb); */
/*     return nmemb; */
/* } */


JNIEXPORT jint JNICALL Java_org_example_Main_foobar
  (JNIEnv *env, jclass cls) // jlong bb_ptr
{
    CURL *curl;
    long http_code = 0;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");

    /* curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb); */
    /* curl_easy_setopt(curl, CURLOPT_WRITEDATA, bb_ptr); */

    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    return (int) http_code;
}
