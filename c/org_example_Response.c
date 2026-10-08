#include <jni.h>
#include "logging.h"
#include "globals.h"
#include "macros.h"
#include "curl/curl.h"

JNIEXPORT jlong JNICALL Java_org_example_Response_from_1curl
  (JNIEnv *env, jobject jresp, jlong jcurl) {

    log_debug("composing in the response");
    CURL *curl = (CURL *) jcurl;
    CURLcode code = CURLE_OK;

    long http_code;
    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (code != CURLE_OK) goto exit;
    log_debug("http code: %ld", http_code);

    char *effective_url = NULL;
    code = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
    if (code != CURLE_OK) goto exit;
    log_debug("http effective url: %s", effective_url);

    double content_length;
    code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
    if (code != CURLE_OK) goto exit;
    log_debug("http content length: %f", content_length);

    log_debug("processing headers");
    {
        struct curl_header *h;
        struct curl_header *prev = NULL;
        jstring jname;
        jstring jvalue;
        do {
            h = curl_easy_nextheader(curl, CURLH_HEADER, -1, prev);
            if (h) {
                log_debug("header %s: %s (%u)", h->name, h->value, (unsigned int) h->amount);
                jname = JNI_CALL(env, NewStringUTF, h->name);
                jvalue = JNI_CALL(env, NewStringUTF, h->value);
                JNI_CALL(env, CallVoidMethod, jresp, _g.Response.addHeader, jname, jvalue);
                log_debug("header set %s: %s", h->name, h->value);
            }
            prev = h;
        } while(h);
    }

    JNI_CALL(env, SetIntField, jresp, _g.Response.status, (int) http_code);
    log_debug("http status is set");

    JNI_CALL(env, SetLongField, jresp, _g.Response.contentLength, (long) content_length);
    log_debug("content length is set");

    if (effective_url) {
        jstring jeffurl = JNI_CALL(env, NewStringUTF, effective_url);
        JNI_CALL(env, SetObjectField, jresp, _g.Response.effectiveUrl, jeffurl);
        log_debug("effective URL is set");
    }

exit:
    if (code != CURLE_OK) {
        log_error("response processing failed with code: %d", code);
    }
    return code;
}
