#include <stdlib.h>
#include <jni.h>
#include <curl/curl.h>

static jmethodID meth_OS_write_BaII;
static jmethodID meth_IS_read_BaII;
static jmethodID meth_WF_handle_BaII;

static int _CURL_JVM_VER = JNI_VERSION_1_8;


jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, _CURL_JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {
        jclass jcls;
        jmethodID jmeth;

        /* OutputStream */
        jcls = (*env)->FindClass(env, "java/io/OutputStream");
        if (jcls == NULL) {
            return JNI_ERR;
        }
        jmeth = (*env)->GetMethodID(env, jcls, "write", "([BII)V");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            meth_OS_write_BaII = jmeth;
        }

        /* InputStream */
        jcls = (*env)->FindClass(env, "java/io/InputStream");
        if (jcls == NULL) {
            return JNI_ERR;
        }
        jmeth = (*env)->GetMethodID(env, jcls, "read", "([BII)I");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            meth_IS_read_BaII = jmeth;
        }

        /* IWriteHandler */
        jcls = (*env)->FindClass(env, "org/example/IWriteHandler");
        if (jcls == NULL) {
            return JNI_ERR;
        }
        jmeth = (*env)->GetMethodID(env, jcls, "handle", "([BII)V");
        if (jmeth == NULL) {
            return JNI_ERR;
        } else {
            meth_WF_handle_BaII = jmeth;
        }

        /* OK */
        return _CURL_JVM_VER;
    }
}


struct user_data {
    size_t i;
    size_t total;
    JNIEnv *env;
    jbyteArray jbuf;
    jobject jobj;
};


struct user_data * make_user_data(JNIEnv *env, jobject jobj) {
    jbyteArray jbuf = (*env)->NewByteArray(env, CURL_MAX_WRITE_SIZE);

    struct user_data * ud = malloc(sizeof(struct user_data));
    ud->i = 0;
    ud->total = 0;
    ud->env = env;
    ud->jbuf = jbuf;
    ud->jobj = (*env)->NewGlobalRef(env, jobj);

    return ud;
}


void clear_user_data(JNIEnv * env, struct user_data * ud) {
    (*env)->DeleteGlobalRef(env, ud->jobj);
    free(ud);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1init
  (JNIEnv *env, jclass jcls) {
    return (jlong) curl_easy_init();
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1URL
  (JNIEnv *env, jclass jcls, jlong jptr, jstring jurl) {
    CURL *curl = (CURL *) jptr;
    const char *url = (*env)->GetStringUTFChars(env, jurl, NULL);
    if (url == NULL) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }
    return curl_easy_setopt(curl, CURLOPT_URL, url);
}

JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1FOLLOWLOCATION
  (JNIEnv *env, jclass jcls, jlong jptr, jint jcode) {
    CURL *curl = (CURL *) jptr;
    return curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, jcode);
}


JNIEXPORT void JNICALL Java_org_example_Native_curl_1easy_1cleanup
  (JNIEnv *env, jclass jcls, jlong jptr) {
    CURL *curl = (CURL *) jptr;
    curl_easy_cleanup(curl);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1perform
  (JNIEnv *env, jclass jcls, jlong jptr) {
    CURL *curl = (CURL *) jptr;
    return curl_easy_perform(curl);
}

static size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    (*env)->SetByteArrayRegion(env, ud->jbuf, 0, total, (jbyte *) data);

    (*env)->CallVoidMethod(env, ud->jobj, meth_OS_write_BaII, ud->jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CURL_WRITEFUNC_ERROR;
    }

    ud->i++;
    ud->total += total;
    return total;
}


static size_t write_callback_handler(char *data, size_t size, size_t nmemb, void *userdata)
{
    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    (*env)->SetByteArrayRegion(env, ud->jbuf, 0, total, (jbyte *) data);

    (*env)->CallVoidMethod(env, ud->jobj, meth_WF_handle_BaII, ud->jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CURL_WRITEFUNC_ERROR;
    }

    ud->i++;
    ud->total += total;
    return total;
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA_1file
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong fp) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL); /* NULL=fwrite */
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1READDATA_1file
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong fp) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL); /* NULL=fread */
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_READDATA, fp);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEDATA_1stream
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong ud_ptr) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_stream);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, ud_ptr);
}



static size_t read_callback_stream(char *data, size_t size, size_t nmemb, void *userdata) {

    size_t total = size * nmemb;
    struct user_data *ud = (struct user_data *) userdata;
    JNIEnv *env = ud->env;

    // TODO: buf size?
    jint jlen = (*env)->CallIntMethod(env, ud->jobj, meth_IS_read_BaII, ud->jbuf, 0, total);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CURL_READFUNC_ABORT;
    }

    // (*env)->SetByteArrayRegion(env, ud->jbuf, 0, jlen, (jbyte *) data);
    // memcpy?

    ud->i++;
    ud->total += jlen;
    return total;


  /* FILE *readhere = (FILE *)userdata; */
  /* curl_off_t nread; */

  /* /\* copy as much data as possible into the 'ptr' buffer, but no more than */
  /*    'size' * 'nmemb' bytes. *\/ */
  /* size_t retcode = fread(ptr, size, nmemb, readhere); */

  /* nread = (curl_off_t)retcode; */

  /* fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T */
  /*         " bytes from file\n", nread); */
  /* return retcode; */
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1READDATA_1stream
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong ud_ptr) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback_stream);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_READDATA, ud_ptr);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_curl_1easy_1setopt_1CURLOPT_1WRITEFUNCTION
  (JNIEnv *env, jclass jcls, jlong jcurl, jlong ud_ptr) {
    CURL *curl = (CURL *) jcurl;
    CURLcode result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_handler);
    if (result != CURLE_OK) {
        return result;
    }
    return curl_easy_setopt(curl, CURLOPT_WRITEDATA, ud_ptr);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_fopen
  (JNIEnv *env, jclass jcls, jstring jpath) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    FILE *fp = fopen(path, "wb");
    return (jlong) fp;
}


JNIEXPORT void JNICALL Java_org_example_Native_fclose
  (JNIEnv *env, jclass jcls, jlong jptr) {
    FILE *fp = (FILE *) jptr;
    fclose(fp);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_init_1write_1data_1stream
  (JNIEnv *env, jclass jcls, jobject joutput_stream) {
    return (jlong) make_user_data(env, joutput_stream);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_init_1read_1data_1stream
  (JNIEnv *env, jclass jcls, jobject jinput_stream) {
    return (jlong) make_user_data(env, jinput_stream);
}


JNIEXPORT void JNICALL Java_org_example_Native_close_1user_1data
  (JNIEnv *env, jclass jcls, jlong jptr) {
    struct user_data *ud = (struct user_data *) jptr;
    clear_user_data(env, ud);
}


JNIEXPORT jlong JNICALL Java_org_example_Native_init_1write_1data_1handler
  (JNIEnv *env, jclass jcls, jobject jhandler) {
    return (jlong) make_user_data(env, jhandler);
}
