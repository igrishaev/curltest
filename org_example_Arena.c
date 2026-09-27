#include <jni.h>
#include <string.h>

char * put_byte(char *bb, char value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}

char * put_int(char *bb, int value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}

char * put_long(char* bb, long value) {
    memcpy(bb, &value, sizeof value);
    return bb += sizeof value;
}


JNIEXPORT jint JNICALL Java_org_example_Arena_initByteBuffer
  (JNIEnv *env, jclass jcls, jobject jbb) {

    char *bb = (char *) (*env)->GetDirectBufferAddress(env, jbb);
    if (bb == NULL) {
        return -1;
    }

    /* TODO: correct 1 */

    bb = put_byte(bb, 1);
    bb = put_long(bb, (long) NULL);
    bb = put_long(bb, (long) bb);

    return 0;
}
