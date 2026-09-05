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

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA
(JNIEnv *env, jclass jcls, jlong jptr, jstring jpath) {
    CURL *curl = (CURL *) jptr;

    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    FILE *fp = fopen(path, "wb");

    if (path == NULL) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }
    /* TODO */
    if (fp == NULL) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* fclose(fp); */

    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
}
