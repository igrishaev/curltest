#ifndef __MACROS_H
#define __MACROS_H

#define J_STRING     "Ljava/lang/String;"
#define J_STRING_ARR "[Ljava/lang/String;"
#define J_INT        "I"
#define J_BOOL       "Z"
#define J_OS         "Ljava/io/OutputStream;"
#define J_BA         "[B"

#define JNI_CALL(env, method, ...) (*env)->method(env, ##__VA_ARGS__)

#endif /* __MACROS_H */
