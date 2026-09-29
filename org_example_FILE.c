#include <jni.h>
#include <stdio.h>

/* enum { */
/*     ERR_FOO, */
/*     ERR_FILE_OPEN */
/* }; */

JNIEXPORT jlong JNICALL Java_org_example_FILE_fopen
  (JNIEnv *env, jclass jcls, jstring jpath, jstring jmode) {

    /* TODO: use error codes */

    const char *path = NULL;
    const char *mode = NULL;
    FILE *fp = NULL;
    int error = 0;

    path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (!path) {
        error = -1;
        goto _cleanup;
    }

    mode = (*env)->GetStringUTFChars(env, jmode, NULL);
    if (!mode) {
        error = -2;
        goto _cleanup;
    }

    fp = fopen(path, mode);
    if (!fp) {
        error = -3;
        goto _cleanup;
    }

    return (jlong) fp;

_cleanup:

    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jmode, mode);

    return error;
}

JNIEXPORT jlong JNICALL Java_org_example_FILE_fclose
  (JNIEnv *env, jclass jcls, jlong jptr) {
    FILE *fp = (FILE *) jptr;
    return fclose(fp);
}
