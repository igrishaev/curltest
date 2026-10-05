#include <jni.h>
#include "macros.h"
#include "debug.h"
#include "accum.h"

JNIEXPORT jlong JNICALL Java_org_example_Accumulator__1allocate
  (JNIEnv *env, jclass jcls, jlong jsize) {
    return (jlong) accum_init(jsize, 2);
}

JNIEXPORT jlong JNICALL Java_org_example_Accumulator__1free
  (JNIEnv *env, jclass jcls, jlong jptr) {
    accum_free((void *)jptr);
    return 0;
}
