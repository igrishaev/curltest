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
    int result = 0;

    path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (!path) {
        result = -1;
        goto exit;
    }

    mode = (*env)->GetStringUTFChars(env, jmode, NULL);
    if (!mode) {
        result = -2;
        goto exit;
    }

    fp = fopen(path, mode);
    if (!fp) {
        result = -3;
        goto exit;
    }

exit:
    if (path) (*env)->ReleaseStringUTFChars(env, jpath, path);
    if (mode) (*env)->ReleaseStringUTFChars(env, jmode, mode);

    if (result == 0) {
        return (jlong) fp;
    } else {
        return result;
    }
}

JNIEXPORT jlong JNICALL Java_org_example_FILE_fclose
  (JNIEnv *env, jclass jcls, jlong jptr) {
    FILE *fp = (FILE *) jptr;
    return fclose(fp); /* TODO: test result here */
}
