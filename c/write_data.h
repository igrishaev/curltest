#ifndef __WRITE_DATA_H__
#define __WRITE_DATA_H__

#include <jni.h>

struct write_data;

struct write_data * write_data_init(JNIEnv *env, jobject jobj);

void write_data_free(struct write_data * wd);

size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata);

#endif /* __WRITE_DATA_H__ */
