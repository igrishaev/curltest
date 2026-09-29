#include <jni.h>
#include <stdio.h>

JNIEXPORT jlong JNICALL Java_org_example_FILE_fopen
  (JNIEnv *env, jclass jcls, jstring jpath, jstring jmode) {

    /* TODO: use error codes */
    /* TODO: free chars */

    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (path == NULL) {
        return -1;
    }

    const char *mode = (*env)->GetStringUTFChars(env, jmode, NULL);
    if (path == NULL) {
        return -2;
    }

    /* TODO */
    /* (*env)->ReleaseStringUTFChars(env, jpath, path) */

    FILE *fp = fopen(path, mode);
    if (fp == NULL) {
        return -3;
    }

    return (jlong) fp;
}

JNIEXPORT jlong JNICALL Java_org_example_FILE_fclose
  (JNIEnv *env, jclass jcls, jlong jptr) {
    FILE *fp = (FILE *) jptr;
    fclose(fp);
    return 0;
}
