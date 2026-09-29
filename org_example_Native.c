#include <stdlib.h>
#include <jni.h>
/* #include <curl/curl.h> */
#include <string.h>
#include <stdio.h>

#include "curl.h"

static jmethodID meth_OS_write_BaII;
static jmethodID meth_IS_read_BaII;
static jmethodID meth_WF_handle_BaII;

static int _CURL_JVM_VER = JNI_VERSION_1_8;


jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, _CURL_JVM_VER) != JNI_OK) {
        return JNI_ERR;
    } else {


        char * version = curl_version();
        printf("version: %s \n", version);

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


JNIEXPORT void JNICALL Java_org_example_Native_read_1curl_1constants
  (JNIEnv *env, jclass jcls, jlong jbb) {
    char *bb = (char *) jbb;

    bb = put_int(bb, CURLOPT_ABSTRACT_UNIX_SOCKET);
    bb = put_int(bb, CURLOPT_ACCEPTTIMEOUT_MS);
    bb = put_int(bb, CURLOPT_ACCEPT_ENCODING);
    bb = put_int(bb, CURLOPT_ADDRESS_SCOPE);
    bb = put_int(bb, CURLOPT_ALTSVC);
    bb = put_int(bb, CURLOPT_ALTSVC_CTRL);
    bb = put_int(bb, CURLOPT_APPEND);
    bb = put_int(bb, CURLOPT_AUTOREFERER);
    bb = put_int(bb, CURLOPT_AWS_SIGV4);
    bb = put_int(bb, CURLOPT_BUFFERSIZE);
    bb = put_int(bb, CURLOPT_CAINFO);
    bb = put_int(bb, CURLOPT_CAINFO_BLOB);
    bb = put_int(bb, CURLOPT_CAPATH);
    bb = put_int(bb, CURLOPT_CA_CACHE_TIMEOUT);
    bb = put_int(bb, CURLOPT_CERTINFO);
    bb = put_int(bb, CURLOPT_CHUNK_BGN_FUNCTION);
    bb = put_int(bb, CURLOPT_CHUNK_DATA);
    bb = put_int(bb, CURLOPT_CHUNK_END_FUNCTION);
    bb = put_int(bb, CURLOPT_CLOSESOCKETDATA);
    bb = put_int(bb, CURLOPT_CLOSESOCKETFUNCTION);
    bb = put_int(bb, CURLOPT_CONNECTTIMEOUT);
    bb = put_int(bb, CURLOPT_CONNECTTIMEOUT_MS);
    bb = put_int(bb, CURLOPT_CONNECT_ONLY);
    bb = put_int(bb, CURLOPT_CONNECT_TO);
    bb = put_int(bb, CURLOPT_CONV_FROM_NETWORK_FUNCTION);
    bb = put_int(bb, CURLOPT_CONV_FROM_UTF8_FUNCTION);
    bb = put_int(bb, CURLOPT_CONV_TO_NETWORK_FUNCTION);
    bb = put_int(bb, CURLOPT_COOKIE);
    bb = put_int(bb, CURLOPT_COOKIEFILE);
    bb = put_int(bb, CURLOPT_COOKIEJAR);
    bb = put_int(bb, CURLOPT_COOKIELIST);
    bb = put_int(bb, CURLOPT_COOKIESESSION);
    bb = put_int(bb, CURLOPT_COPYPOSTFIELDS);
    bb = put_int(bb, CURLOPT_CRLF);
    bb = put_int(bb, CURLOPT_CRLFILE);
    bb = put_int(bb, CURLOPT_CURLU);
    bb = put_int(bb, CURLOPT_CUSTOMREQUEST);
    bb = put_int(bb, CURLOPT_DEBUGDATA);
    bb = put_int(bb, CURLOPT_DEBUGFUNCTION);
    bb = put_int(bb, CURLOPT_DEFAULT_PROTOCOL);
    bb = put_int(bb, CURLOPT_DIRLISTONLY);
    bb = put_int(bb, CURLOPT_DISALLOW_USERNAME_IN_URL);
    bb = put_int(bb, CURLOPT_DNS_CACHE_TIMEOUT);
    bb = put_int(bb, CURLOPT_DNS_INTERFACE);
    bb = put_int(bb, CURLOPT_DNS_LOCAL_IP4);
    bb = put_int(bb, CURLOPT_DNS_LOCAL_IP6);
    bb = put_int(bb, CURLOPT_DNS_SERVERS);
    bb = put_int(bb, CURLOPT_DNS_SHUFFLE_ADDRESSES);
    bb = put_int(bb, CURLOPT_DNS_USE_GLOBAL_CACHE);
    bb = put_int(bb, CURLOPT_DOH_SSL_VERIFYHOST);
    bb = put_int(bb, CURLOPT_DOH_SSL_VERIFYPEER);
    bb = put_int(bb, CURLOPT_DOH_SSL_VERIFYSTATUS);
    bb = put_int(bb, CURLOPT_DOH_URL);
    bb = put_int(bb, CURLOPT_ECH);
    bb = put_int(bb, CURLOPT_EGDSOCKET);
    bb = put_int(bb, CURLOPT_ERRORBUFFER);
    bb = put_int(bb, CURLOPT_EXPECT_100_TIMEOUT_MS);
    bb = put_int(bb, CURLOPT_FAILONERROR);
    bb = put_int(bb, CURLOPT_FILETIME);
    bb = put_int(bb, CURLOPT_FNMATCH_DATA);
    bb = put_int(bb, CURLOPT_FNMATCH_FUNCTION);
    bb = put_int(bb, CURLOPT_FOLLOWLOCATION);
    bb = put_int(bb, CURLOPT_FORBID_REUSE);
    bb = put_int(bb, CURLOPT_FRESH_CONNECT);
    bb = put_int(bb, CURLOPT_FTPPORT);
    bb = put_int(bb, CURLOPT_FTPSSLAUTH);
    bb = put_int(bb, CURLOPT_FTP_ACCOUNT);
    bb = put_int(bb, CURLOPT_FTP_ALTERNATIVE_TO_USER);
    bb = put_int(bb, CURLOPT_FTP_CREATE_MISSING_DIRS);
    bb = put_int(bb, CURLOPT_FTP_FILEMETHOD);
    bb = put_int(bb, CURLOPT_FTP_SKIP_PASV_IP);
    bb = put_int(bb, CURLOPT_FTP_SSL_CCC);
    bb = put_int(bb, CURLOPT_FTP_USE_EPRT);
    bb = put_int(bb, CURLOPT_FTP_USE_EPSV);
    bb = put_int(bb, CURLOPT_FTP_USE_PRET);
    bb = put_int(bb, CURLOPT_GSSAPI_DELEGATION);
    bb = put_int(bb, CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS);
    bb = put_int(bb, CURLOPT_HAPROXYPROTOCOL);
    bb = put_int(bb, CURLOPT_HAPROXY_CLIENT_IP);
    bb = put_int(bb, CURLOPT_HEADER);
    bb = put_int(bb, CURLOPT_HEADERDATA);
    bb = put_int(bb, CURLOPT_HEADERFUNCTION);
    bb = put_int(bb, CURLOPT_HEADEROPT);
    bb = put_int(bb, CURLOPT_HSTS);
    bb = put_int(bb, CURLOPT_HSTSREADDATA);
    bb = put_int(bb, CURLOPT_HSTSREADFUNCTION);
    bb = put_int(bb, CURLOPT_HSTSWRITEDATA);
    bb = put_int(bb, CURLOPT_HSTSWRITEFUNCTION);
    bb = put_int(bb, CURLOPT_HSTS_CTRL);
    bb = put_int(bb, CURLOPT_HTTP09_ALLOWED);
    bb = put_int(bb, CURLOPT_HTTP200ALIASES);
    bb = put_int(bb, CURLOPT_HTTPAUTH);
    bb = put_int(bb, CURLOPT_HTTPGET);
    bb = put_int(bb, CURLOPT_HTTPHEADER);
    bb = put_int(bb, CURLOPT_HTTPPOST);
    bb = put_int(bb, CURLOPT_HTTPPROXYTUNNEL);
    bb = put_int(bb, CURLOPT_HTTPSIG_ALGORITHM);
    bb = put_int(bb, CURLOPT_HTTPSIG_HEADERS);
    bb = put_int(bb, CURLOPT_HTTPSIG_KEY);
    bb = put_int(bb, CURLOPT_HTTPSIG_KEYID);
    bb = put_int(bb, CURLOPT_HTTP_CONTENT_DECODING);
    bb = put_int(bb, CURLOPT_HTTP_TRANSFER_DECODING);
    bb = put_int(bb, CURLOPT_HTTP_VERSION);
    bb = put_int(bb, CURLOPT_IGNORE_CONTENT_LENGTH);
    bb = put_int(bb, CURLOPT_INFILESIZE);
    bb = put_int(bb, CURLOPT_INFILESIZE_LARGE);
    bb = put_int(bb, CURLOPT_INTERFACE);
    bb = put_int(bb, CURLOPT_INTERLEAVEDATA);
    bb = put_int(bb, CURLOPT_INTERLEAVEFUNCTION);
    bb = put_int(bb, CURLOPT_IOCTLDATA);
    bb = put_int(bb, CURLOPT_IOCTLFUNCTION);
    bb = put_int(bb, CURLOPT_IPRESOLVE);
    bb = put_int(bb, CURLOPT_ISSUERCERT);
    bb = put_int(bb, CURLOPT_ISSUERCERT_BLOB);
    bb = put_int(bb, CURLOPT_KEEP_SENDING_ON_ERROR);
    bb = put_int(bb, CURLOPT_KEYPASSWD);
    bb = put_int(bb, CURLOPT_KRBLEVEL);
    bb = put_int(bb, CURLOPT_LOCALPORT);
    bb = put_int(bb, CURLOPT_LOCALPORTRANGE);
    bb = put_int(bb, CURLOPT_LOGIN_OPTIONS);
    bb = put_int(bb, CURLOPT_LOW_SPEED_LIMIT);
    bb = put_int(bb, CURLOPT_LOW_SPEED_TIME);
    bb = put_int(bb, CURLOPT_MAIL_AUTH);
    bb = put_int(bb, CURLOPT_MAIL_FROM);
    bb = put_int(bb, CURLOPT_MAIL_RCPT);
    bb = put_int(bb, CURLOPT_MAIL_RCPT_ALLOWFAILS);
    bb = put_int(bb, CURLOPT_MAXAGE_CONN);
    bb = put_int(bb, CURLOPT_MAXCONNECTS);
    bb = put_int(bb, CURLOPT_MAXFILESIZE);
    bb = put_int(bb, CURLOPT_MAXFILESIZE_LARGE);
    bb = put_int(bb, CURLOPT_MAXLIFETIME_CONN);
    bb = put_int(bb, CURLOPT_MAXREDIRS);
    bb = put_int(bb, CURLOPT_MAX_RECV_SPEED_LARGE);
    bb = put_int(bb, CURLOPT_MAX_SEND_SPEED_LARGE);
    bb = put_int(bb, CURLOPT_MIMEPOST);
    bb = put_int(bb, CURLOPT_MIME_OPTIONS);
    bb = put_int(bb, CURLOPT_NETRC);
    bb = put_int(bb, CURLOPT_NETRC_FILE);
    bb = put_int(bb, CURLOPT_NEW_DIRECTORY_PERMS);
    bb = put_int(bb, CURLOPT_NEW_FILE_PERMS);
    bb = put_int(bb, CURLOPT_NOBODY);
    bb = put_int(bb, CURLOPT_NOPROGRESS);
    bb = put_int(bb, CURLOPT_NOPROXY);
    bb = put_int(bb, CURLOPT_NOSIGNAL);
    bb = put_int(bb, CURLOPT_OPENSOCKETDATA);
    bb = put_int(bb, CURLOPT_OPENSOCKETFUNCTION);
    bb = put_int(bb, CURLOPT_PASSWORD);
    bb = put_int(bb, CURLOPT_PATH_AS_IS);
    bb = put_int(bb, CURLOPT_PINNEDPUBLICKEY);
    bb = put_int(bb, CURLOPT_PIPEWAIT);
    bb = put_int(bb, CURLOPT_PORT);
    bb = put_int(bb, CURLOPT_POST);
    bb = put_int(bb, CURLOPT_POSTFIELDS);
    bb = put_int(bb, CURLOPT_POSTFIELDSIZE);
    bb = put_int(bb, CURLOPT_POSTFIELDSIZE_LARGE);
    bb = put_int(bb, CURLOPT_POSTQUOTE);
    bb = put_int(bb, CURLOPT_POSTREDIR);
    bb = put_int(bb, CURLOPT_PREQUOTE);
    bb = put_int(bb, CURLOPT_PREREQDATA);
    bb = put_int(bb, CURLOPT_PREREQFUNCTION);
    bb = put_int(bb, CURLOPT_PRE_PROXY);
    bb = put_int(bb, CURLOPT_PRIVATE);
    bb = put_int(bb, CURLOPT_PROGRESSDATA);
    bb = put_int(bb, CURLOPT_PROGRESSFUNCTION);
    bb = put_int(bb, CURLOPT_PROTOCOLS);
    bb = put_int(bb, CURLOPT_PROTOCOLS_STR);
    bb = put_int(bb, CURLOPT_PROXY);
    bb = put_int(bb, CURLOPT_PROXYAUTH);
    bb = put_int(bb, CURLOPT_PROXYHEADER);
    bb = put_int(bb, CURLOPT_PROXYPASSWORD);
    bb = put_int(bb, CURLOPT_PROXYPORT);
    bb = put_int(bb, CURLOPT_PROXYTYPE);
    bb = put_int(bb, CURLOPT_PROXYUSERNAME);
    bb = put_int(bb, CURLOPT_PROXYUSERPWD);
    bb = put_int(bb, CURLOPT_PROXY_CAINFO);
    bb = put_int(bb, CURLOPT_PROXY_CAINFO_BLOB);
    bb = put_int(bb, CURLOPT_PROXY_CAPATH);
    bb = put_int(bb, CURLOPT_PROXY_CRLFILE);
    bb = put_int(bb, CURLOPT_PROXY_ISSUERCERT);
    bb = put_int(bb, CURLOPT_PROXY_ISSUERCERT_BLOB);
    bb = put_int(bb, CURLOPT_PROXY_KEYPASSWD);
    bb = put_int(bb, CURLOPT_PROXY_PINNEDPUBLICKEY);
    bb = put_int(bb, CURLOPT_PROXY_SERVICE_NAME);
    bb = put_int(bb, CURLOPT_PROXY_SSLCERT);
    bb = put_int(bb, CURLOPT_PROXY_SSLCERTTYPE);
    bb = put_int(bb, CURLOPT_PROXY_SSLCERT_BLOB);
    bb = put_int(bb, CURLOPT_PROXY_SSLKEY);
    bb = put_int(bb, CURLOPT_PROXY_SSLKEYTYPE);
    bb = put_int(bb, CURLOPT_PROXY_SSLKEY_BLOB);
    bb = put_int(bb, CURLOPT_PROXY_SSLVERSION);
    bb = put_int(bb, CURLOPT_PROXY_SSL_CIPHER_LIST);
    bb = put_int(bb, CURLOPT_PROXY_SSL_OPTIONS);
    bb = put_int(bb, CURLOPT_PROXY_SSL_VERIFYHOST);
    bb = put_int(bb, CURLOPT_PROXY_SSL_VERIFYPEER);
    bb = put_int(bb, CURLOPT_PROXY_TLS13_CIPHERS);
    bb = put_int(bb, CURLOPT_PROXY_TLSAUTH_PASSWORD);
    bb = put_int(bb, CURLOPT_PROXY_TLSAUTH_TYPE);
    bb = put_int(bb, CURLOPT_PROXY_TLSAUTH_USERNAME);
    bb = put_int(bb, CURLOPT_PROXY_TRANSFER_MODE);
    bb = put_int(bb, CURLOPT_PUT);
    bb = put_int(bb, CURLOPT_QUICK_EXIT);
    bb = put_int(bb, CURLOPT_QUOTE);
    bb = put_int(bb, CURLOPT_RANDOM_FILE);
    bb = put_int(bb, CURLOPT_RANGE);
    bb = put_int(bb, CURLOPT_READDATA);
    bb = put_int(bb, CURLOPT_READFUNCTION);
    bb = put_int(bb, CURLOPT_REDIR_PROTOCOLS);
    bb = put_int(bb, CURLOPT_REDIR_PROTOCOLS_STR);
    bb = put_int(bb, CURLOPT_REFERER);
    bb = put_int(bb, CURLOPT_REQUEST_TARGET);
    bb = put_int(bb, CURLOPT_RESOLVE);
    bb = put_int(bb, CURLOPT_RESOLVER_START_DATA);
    bb = put_int(bb, CURLOPT_RESOLVER_START_FUNCTION);
    bb = put_int(bb, CURLOPT_RESUME_FROM);
    bb = put_int(bb, CURLOPT_RESUME_FROM_LARGE);
    bb = put_int(bb, CURLOPT_RTSP_CLIENT_CSEQ);
    bb = put_int(bb, CURLOPT_RTSP_REQUEST);
    bb = put_int(bb, CURLOPT_RTSP_SERVER_CSEQ);
    bb = put_int(bb, CURLOPT_RTSP_SESSION_ID);
    bb = put_int(bb, CURLOPT_RTSP_STREAM_URI);
    bb = put_int(bb, CURLOPT_RTSP_TRANSPORT);
    bb = put_int(bb, CURLOPT_SASL_AUTHZID);
    bb = put_int(bb, CURLOPT_SASL_IR);
    bb = put_int(bb, CURLOPT_SEEKDATA);
    bb = put_int(bb, CURLOPT_SEEKFUNCTION);
    bb = put_int(bb, CURLOPT_SERVER_RESPONSE_TIMEOUT);
    bb = put_int(bb, CURLOPT_SERVER_RESPONSE_TIMEOUT_MS);
    bb = put_int(bb, CURLOPT_SERVICE_NAME);
    bb = put_int(bb, CURLOPT_SHARE);
    bb = put_int(bb, CURLOPT_SOCKOPTDATA);
    bb = put_int(bb, CURLOPT_SOCKOPTFUNCTION);
    bb = put_int(bb, CURLOPT_SOCKS5_AUTH);
    bb = put_int(bb, CURLOPT_SOCKS5_GSSAPI_NEC);
    bb = put_int(bb, CURLOPT_SOCKS5_GSSAPI_SERVICE);
    bb = put_int(bb, CURLOPT_SSH_AUTH_TYPES);
    bb = put_int(bb, CURLOPT_SSH_COMPRESSION);
    bb = put_int(bb, CURLOPT_SSH_HOSTKEYDATA);
    bb = put_int(bb, CURLOPT_SSH_HOSTKEYFUNCTION);
    bb = put_int(bb, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5);
    bb = put_int(bb, CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256);
    bb = put_int(bb, CURLOPT_SSH_KEYDATA);
    bb = put_int(bb, CURLOPT_SSH_KEYFUNCTION);
    bb = put_int(bb, CURLOPT_SSH_KNOWNHOSTS);
    bb = put_int(bb, CURLOPT_SSH_PRIVATE_KEYFILE);
    bb = put_int(bb, CURLOPT_SSH_PUBLIC_KEYFILE);
    bb = put_int(bb, CURLOPT_SSLCERT);
    bb = put_int(bb, CURLOPT_SSLCERTTYPE);
    bb = put_int(bb, CURLOPT_SSLCERT_BLOB);
    bb = put_int(bb, CURLOPT_SSLENGINE);
    bb = put_int(bb, CURLOPT_SSLENGINE_DEFAULT);
    bb = put_int(bb, CURLOPT_SSLKEY);
    bb = put_int(bb, CURLOPT_SSLKEYTYPE);
    bb = put_int(bb, CURLOPT_SSLKEY_BLOB);
    bb = put_int(bb, CURLOPT_SSLVERSION);
    bb = put_int(bb, CURLOPT_SSL_CIPHER_LIST);
    bb = put_int(bb, CURLOPT_SSL_CTX_DATA);
    bb = put_int(bb, CURLOPT_SSL_CTX_FUNCTION);
    bb = put_int(bb, CURLOPT_SSL_EC_CURVES);
    bb = put_int(bb, CURLOPT_SSL_ENABLE_ALPN);
    bb = put_int(bb, CURLOPT_SSL_ENABLE_NPN);
    bb = put_int(bb, CURLOPT_SSL_FALSESTART);
    bb = put_int(bb, CURLOPT_SSL_OPTIONS);
    bb = put_int(bb, CURLOPT_SSL_SESSIONID_CACHE);
    bb = put_int(bb, CURLOPT_SSL_SIGNATURE_ALGORITHMS);
    bb = put_int(bb, CURLOPT_SSL_VERIFYHOST);
    bb = put_int(bb, CURLOPT_SSL_VERIFYPEER);
    bb = put_int(bb, CURLOPT_SSL_VERIFYSTATUS);
    bb = put_int(bb, CURLOPT_STDERR);
    bb = put_int(bb, CURLOPT_STREAM_DEPENDS);
    bb = put_int(bb, CURLOPT_STREAM_DEPENDS_E);
    bb = put_int(bb, CURLOPT_STREAM_WEIGHT);
    bb = put_int(bb, CURLOPT_SUPPRESS_CONNECT_HEADERS);
    bb = put_int(bb, CURLOPT_TCP_FASTOPEN);
    bb = put_int(bb, CURLOPT_TCP_KEEPALIVE);
    bb = put_int(bb, CURLOPT_TCP_KEEPCNT);
    bb = put_int(bb, CURLOPT_TCP_KEEPIDLE);
    bb = put_int(bb, CURLOPT_TCP_KEEPINTVL);
    bb = put_int(bb, CURLOPT_TCP_NODELAY);
    bb = put_int(bb, CURLOPT_TELNETOPTIONS);
    bb = put_int(bb, CURLOPT_TFTP_BLKSIZE);
    bb = put_int(bb, CURLOPT_TFTP_NO_OPTIONS);
    bb = put_int(bb, CURLOPT_TIMECONDITION);
    bb = put_int(bb, CURLOPT_TIMEOUT);
    bb = put_int(bb, CURLOPT_TIMEOUT_MS);
    bb = put_int(bb, CURLOPT_TIMEVALUE);
    bb = put_int(bb, CURLOPT_TIMEVALUE_LARGE);
    bb = put_int(bb, CURLOPT_TLS13_CIPHERS);
    bb = put_int(bb, CURLOPT_TLSAUTH_PASSWORD);
    bb = put_int(bb, CURLOPT_TLSAUTH_TYPE);
    bb = put_int(bb, CURLOPT_TLSAUTH_USERNAME);
    bb = put_int(bb, CURLOPT_TRAILERDATA);
    bb = put_int(bb, CURLOPT_TRAILERFUNCTION);
    bb = put_int(bb, CURLOPT_TRANSFERTEXT);
    bb = put_int(bb, CURLOPT_TRANSFER_ENCODING);
    bb = put_int(bb, CURLOPT_UNIX_SOCKET_PATH);
    bb = put_int(bb, CURLOPT_UNRESTRICTED_AUTH);
    bb = put_int(bb, CURLOPT_UPKEEP_INTERVAL_MS);
    bb = put_int(bb, CURLOPT_UPLOAD);
    bb = put_int(bb, CURLOPT_UPLOAD_BUFFERSIZE);
    bb = put_int(bb, CURLOPT_UPLOAD_FLAGS);
    bb = put_int(bb, CURLOPT_URL);
    bb = put_int(bb, CURLOPT_USERAGENT);
    bb = put_int(bb, CURLOPT_USERNAME);
    bb = put_int(bb, CURLOPT_USERPWD);
    bb = put_int(bb, CURLOPT_USE_SSL);
    bb = put_int(bb, CURLOPT_VERBOSE);
    bb = put_int(bb, CURLOPT_WILDCARDMATCH);
    bb = put_int(bb, CURLOPT_WRITEDATA);
    bb = put_int(bb, CURLOPT_WRITEFUNCTION);
    bb = put_int(bb, CURLOPT_WS_OPTIONS);
    bb = put_int(bb, CURLOPT_XFERINFODATA);
    bb = put_int(bb, CURLOPT_XFERINFOFUNCTION);
    bb = put_int(bb, CURLOPT_XOAUTH2_BEARER);

    bb = put_int(bb, CURLFOLLOW_ALL);
    bb = put_int(bb, CURLFOLLOW_OBEYCODE);
    bb = put_int(bb, CURLFOLLOW_FIRSTONLY);

}

JNIEXPORT jint JNICALL Java_org_example_Native_perform
  (JNIEnv *env, jclass jcls, jlong jbb) {

    size_t *bb = (size_t *) jbb;

    CURL *curl = curl_easy_init();
    size_t counter = bb[0];
    size_t opt;
    size_t val;
    CURLcode res;
    int i;

    fprintf(stdout, "counter: %lu \n", counter);

    for (i = 0; i < counter; i++) {
        opt = bb[1 + i * 2];
        val = bb[1 + i * 2 + 1];

        fprintf(stdout, "opt: %lu, val: %lu \n", opt, val);

        res = curl_easy_setopt(curl, opt, val);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_setopt() failed: %s\n", curl_easy_strerror(res));
            return res;
        }
    }

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);

    return res;
}
