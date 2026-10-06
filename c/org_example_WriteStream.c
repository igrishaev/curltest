#include <jni.h>
#include "write_data.h"


JNIEXPORT jlong JNICALL Java_org_example_WriteStream__1allocate
  (JNIEnv *env, jclass jcls, jobject jstream) {
    return (jlong) write_data_init(env, jstream);
}


JNIEXPORT jlong JNICALL Java_org_example_WriteStream__1free
  (JNIEnv *env, jclass jobj, jlong jptr) {
    void *ptr = (void *) jptr;
    write_data_free(ptr);
    return 0;
}
