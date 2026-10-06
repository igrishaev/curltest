#include <jni.h>
#include "write_data.h"

JNIEXPORT jlong JNICALL Java_org_example_WriteCallback__1allocate
  (JNIEnv *env, jclass jcls, jobject jhandler) {
    return (jlong) write_data_init(env, jhandler);
}

JNIEXPORT jlong JNICALL Java_org_example_WriteCallback__1free
  (JNIEnv *env, jclass jcls, jlong jptr) {
    void *ptr = (void *) jptr;
    write_data_free(ptr);
    return 0;
}
