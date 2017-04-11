#ifndef _H_NATIVE_LOG_H_
#define _H_NATIVE_LOG_H_

#include <stdarg.h>
#include <android/log.h>
#include <jni.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Player_Jni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Player_Jni", __VA_ARGS__);
/*
void jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    jclass clazz = env->FindClass(className);
    if (!clazz) {
        LOGE("Unable to find exception class %s", className);
        return;
    }

    if (env->ThrowNew(clazz, msg) != JNI_OK) {
        LOGE("Failed throwing '%s' '%s'", className, msg);
    }
    env->DeleteLocalRef(clazz);
}

void doThrowIAE(JNIEnv* env, const char* msg) {
    jniThrowException(env, "java/lang/IllegalArgumentException", msg);
}*/

#endif
