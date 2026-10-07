#ifndef __CURL_DATA_H__
#define __CURL_DATA_H__

#include <jni.h>

struct curl_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject jobj;
};

struct curl_data * curl_data_init(JNIEnv *env, jobject jobj);

void curl_data_free(struct curl_data * cd);

#endif /* __CURL_DATA_H__ */
