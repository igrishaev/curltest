#include <jni.h>

JNIEXPORT jlong JNICALL Java_org_example_Native_get_1null
  (JNIEnv *env, jclass jcls) {
    return (jlong) NULL;
}
