#ifndef __MACROS_H
#define __MACROS_H

#define J_STRING     "Ljava/lang/String;"
#define J_STRING_ARR "[Ljava/lang/String;"
#define J_INT        "I"
#define J_BOOL       "Z"
#define J_OS         "Ljava/io/OutputStream;"
#define J_BA         "[B"

#define JNI_CALL(env, method, ...) (*env)->method(env, ##__VA_ARGS__)

#define GET_CLASS(env, clsname, clsvar) \
    clsvar = JNI_CALL(env, FindClass, clsname); \
    if (!clsvar) { \
        debug("failed to find class: " clsname); \
        return JNI_ERR; \
    }

#define SET_FIELD(env, jcls, fname, ftype, fvar) \
    fvar = JNI_CALL(env, GetFieldID, jcls, fname, ftype); \
    if (!fvar) { \
        debug("failed to find field: " fname " " ftype); \
        return JNI_ERR; \
    }

#define GET_METHOD(env, jcls, name, sig, var) \
    var = JNI_CALL(env, GetMethodID, jcls, name, sig); \
    if (!var) { \
        debug("failed to find method: " name " " sig); \
        return JNI_ERR; \
    }

#endif /* __MACROS_H */
