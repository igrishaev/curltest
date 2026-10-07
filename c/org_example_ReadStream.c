#include <jni.h>
#include "read_data.h"


JNIEXPORT jlong JNICALL Java_org_example_ReadStream__1allocate
  (JNIEnv *, jclass, jobject) {
    return (jlong) read_data_init(env, jstream);
}


JNIEXPORT jlong JNICALL Java_org_example_ReadStream__1free
  (JNIEnv *, jclass, jlong) {
    void *ptr = (void *) jptr;
    read_data_free(ptr);
    return 0;
}
