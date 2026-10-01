#include <jni.h>
#include "curl.h"

JNIEXPORT jlong JNICALL Java_org_example_Curl3_perform (
    JNIEnv *env,
    jclass jcls,
    jstring jurl,
    jint jmethod,
    jint jfollowLocation,
    jobjectArray jheaders
)
{
    int i;
    long code;
    char *header;
    jstring jheader;
    struct curl_slist *headers = NULL;

    CURL *curl = curl_easy_init();

    /* URL */
    const char *url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (!url) {
        code = -1;
        goto exit;
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (code != CURLE_OK) goto exit;

    (*env)->ReleaseStringUTFChars(env, jurl, url);

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

    /* follow location */
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jfollowLocation);
    if (code != CURLE_OK) goto exit;

    /* headers */
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

    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) goto exit;

    /* perform */
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) goto exit;

exit:

    if (curl) curl_easy_cleanup(curl);
    return code;
}
