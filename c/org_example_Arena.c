#include <jni.h>
#include <string.h>

static char * put_byte(char *bb, char value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}

static char * put_int(char *bb, int value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}

static char * put_long(char* bb, long value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}

JNIEXPORT jint JNICALL Java_org_example_Arena__1init_1byte_1buffer
  (JNIEnv *env, jclass jcls, jobject jbb) {

    void *addr = (*env)->GetDirectBufferAddress(env, jbb);
    if (!addr) {
        return -1;
    }

    char *bb = (char *) addr;
    bb = put_byte(bb, 1);
    bb = put_long(bb, (long) NULL);
    bb = put_long(bb, (long) addr);

    return 0;
}
