#ifndef PIC_LEFT_H
#define PIC_LEFT_H

#include <jni.h>
#include "gl/gl_renderer.h"
#include "gl/gl_picture.h"
#include "gl/gl_yuv.h"
#include "gl/gl_play_yuv.h"
#include "bean/bean.h"
#include "transform/transform.h"
#include "security/licence.h"

jmethodID midIsLeft;
jmethodID midIsUseBitmap;
jmethodID midCtrlStyle;
jmethodID midShowMode;
jmethodID midResolutionRatio;

extern "C" {
// left
JNIEXPORT jint JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnSurfaceCreated(JNIEnv *env,
                                                                                jobject obj);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnSurfaceChanged(JNIEnv *env,
                                                                                jobject obj,
                                                                                jint width,
                                                                                jint height);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnDrawFrame(JNIEnv *env,
                                                                           jobject obj,
                                                                           jobject bmp);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeSetSettingsBean(JNIEnv *env,
                                                                               jobject obj,
                                                                               jobject bean);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeInitApi(JNIEnv *env,
                                                                       jobject obj,
                                                                       jobject bean);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeReleaseApi(JNIEnv *env,
                                                                          jobject obj);
JNIEXPORT jboolean JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnTouch(JNIEnv *env,
                                                                           jobject obj,
                                                                           jint action,
                                                                           jint pointCount,
                                                                           jfloat x1,
                                                                           jfloat y1,
                                                                           jfloat x2,
                                                                           jfloat y2);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkSensor_nativeOnSensor(JNIEnv *env,
                                                                       jobject obj,
                                                                       jfloat x,
                                                                       jfloat y,
                                                                       jfloat z,
                                                                       jlong timestamp);
JNIEXPORT void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeResetTransform(JNIEnv *env,
                                                                              jobject obj);

// licence
JNIEXPORT jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetEncodeA(JNIEnv *,
                                                                             jobject);
JNIEXPORT jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetEncodeH(JNIEnv *,
                                                                             jobject,
                                                                             jstring);
JNIEXPORT jboolean JNICALL Java_com_wq_player_ndk_NdkLicence_nativeIsAllow(JNIEnv *,
                                                                           jobject,
                                                                           jstring,
                                                                           jstring,
                                                                           jstring);
JNIEXPORT jboolean JNICALL Java_com_wq_player_ndk_NdkLicence_nativeHasLicence(JNIEnv *,
                                                                              jobject);
};
#endif //PIC_LEFT_H
