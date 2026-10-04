#include <jni.h>

#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "curl/curl.h"

JNIEXPORT jlong JNICALL Java_org_example_Response_from_1curl
  (JNIEnv *env, jobject jresp, jlong jcurl) {


    debug("composing in the response");
    CURL *curl = (CURL *) jcurl;
    CURLcode code = 0;

    long http_code;
    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (code != CURLE_OK) goto exit;
    debug("http code: %l", http_code);

    char *effective_url = NULL;
    code = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
    if (code != CURLE_OK) goto exit;
    debug("http effective url: %s", effective_url);

    double content_length;
    code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
    if (code != CURLE_OK) goto exit;
    debug("http content length: %d", content_length);

    debug("processing headers");
    {
        struct curl_header *h;
        struct curl_header *prev = NULL;
        do {
            h = curl_easy_nextheader(curl, CURLH_HEADER, -1, prev);
            if (h) {
                debug("header %s: %s (%u)", h->name, h->value, (unsigned int)h->amount);
            }
            prev = h;
        } while(h);
    }

    debug("setting fields");
    /* JNI_CALL(jresp, SetIntField,    jresp, _g.Response.status,        4); */
    /* JNI_CALL(jresp, SetObjectField, jresp, _g.Response.headers,       3); */
    /* JNI_CALL(jresp, SetObjectField, jresp, _g.Response.body,          3); */
    /* JNI_CALL(jresp, SetIntField,    jresp, _g.Response.contentLength, 1); */
    /* JNI_CALL(jresp, SetObjectField, jresp, _g.Response.effectiveUrl,  2); */

exit:
    return code;

}
