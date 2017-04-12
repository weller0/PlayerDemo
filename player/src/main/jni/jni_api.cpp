#include <bean/bean_base.h>
#include "jni_api.h"

GLRenderer *pGLDisplay = NULL;
Transform *mTransform = NULL;
Bean *mBean = NULL;

Licence *mLicence = NULL;
jboolean bHaveLicence = JNI_FALSE;

SettingsBean cpp2JavaForSettingsBean(JNIEnv *env, jobject bean) {
    SettingsBean settingsBean;
    settingsBean.isUseBitmap = (GLboolean) env->CallBooleanMethod(bean, midIsUseBitmap);
    settingsBean.isLeft = (GLboolean) env->CallBooleanMethod(bean, midIsLeft);
    settingsBean.mShowMode = (GLuint) env->CallIntMethod(bean, midShowMode);
    settingsBean.mCtrlStyle = (GLuint) env->CallIntMethod(bean, midCtrlStyle);
    settingsBean.mResolutionRatio = (GLuint) env->CallIntMethod(bean, midResolutionRatio);
    return settingsBean;
}

jint JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnSurfaceCreated(JNIEnv *env,
                                                                      jobject obj) {
    return pGLDisplay->onSurfaceCreated();
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnSurfaceChanged(JNIEnv *env,
                                                                      jobject obj,
                                                                      jint width,
                                                                      jint height) {
    pGLDisplay->onSurfaceChanged(width, height);
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnDrawFrame(JNIEnv *env,
                                                                 jobject obj,
                                                                 jobject bmp) {
    if (bHaveLicence) {
        if (bmp != NULL) {
            AndroidBitmapInfo bitmapInfo;
            int ret;
            if ((ret = AndroidBitmap_getInfo(env, bmp, &bitmapInfo)) < 0) {
                LOGE("[jni_api]AndroidBitmap_getInfo() failed ! error=%d", ret);
                return;
            }
            if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                LOGE("[jni_api]Bitmap format is not RGBA_8888!");
                return;
            }

            Bitmap *bitmap = new Bitmap;
            if ((ret = AndroidBitmap_lockPixels(env, bmp, (void **) (&(bitmap->pixels)))) < 0) {
                LOGE("[jni_api]AndroidBitmap_lockPixels() failed ! error=%d", ret);
                return;
            }
            AndroidBitmap_unlockPixels(env, bmp);
            bitmap->bitmapInfo = bitmapInfo;
            pGLDisplay->onDrawFrame(bitmap);
            delete bitmap;
        } else {
            pGLDisplay->onDrawFrame(NULL);
        }
    }
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeSetSettingsBean(JNIEnv *env,
                                                                     jobject obj,
                                                                     jobject bean) {
    LOGI("[jni_api]setSettingsBean");
    mBean->setSettingsBean(pGLDisplay, mTransform, cpp2JavaForSettingsBean(env, bean));
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeInitApi(JNIEnv *env,
                                                             jobject obj,
                                                             jobject bean) {
    jclass clsSettingsBean = env->FindClass("com/wq/player/bean/SettingsBean");
    midIsUseBitmap = env->GetMethodID(clsSettingsBean, "isUseBitmap", "()Z");
    midIsLeft = env->GetMethodID(clsSettingsBean, "isLeft", "()Z");
    midShowMode = env->GetMethodID(clsSettingsBean, "getShowMode", "()I");
    midCtrlStyle = env->GetMethodID(clsSettingsBean, "getCtrlStyle", "()I");
    midResolutionRatio = env->GetMethodID(clsSettingsBean, "getResolutionRatio", "()I");
    mLicence = new Licence();
    mBean = new Bean(cpp2JavaForSettingsBean(env, bean));
    mTransform = new Transform(mBean->getTransformBean(), mBean->getSettingsBean());
    if(mBean->getSettingsBean()->isUseBitmap) {
        pGLDisplay = new Picture(mBean->getTransformBean(), mBean->getSettingsBean());
    } else {
        pGLDisplay = new Video(mBean->getTransformBean(), mBean->getSettingsBean());
    }
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeReleaseApi(JNIEnv *env,
                                                                jobject obj) {
    LOGI("[jni_api]release");
    if (pGLDisplay != NULL) {
        delete pGLDisplay;
    }
    if (mTransform != NULL) {
        delete mTransform;
    }
    if (mBean != NULL) {
        delete mBean;
    }
    if (mLicence != NULL) {
        delete mLicence;
    }
}

jboolean JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeOnTouch(JNIEnv *env,
                                                                 jobject obj,
                                                                 jint action,
                                                                 jint pointCount,
                                                                 jfloat x1,
                                                                 jfloat y1,
                                                                 jfloat x2,
                                                                 jfloat y2) {
    //LOGI("onTouch action=%d, count=%d, 1(%f, %f), 2(%f, %f)", action, pointCount, x1, y1, x2, y2);
    mTransform->onTouch(action, pointCount, x1, y1, x2, y2);
    return false;
}

void JNICALL Java_com_wq_player_ndk_NdkSensor_nativeOnSensor(JNIEnv *env,
                                                             jobject obj,
                                                             jfloat x,
                                                             jfloat y,
                                                             jfloat z,
                                                             jlong timestamp) {
    mTransform->onSensor(x, y, z, timestamp);
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeResetTransform(JNIEnv *env,
                                                                    jobject obj) {
    mTransform->reset();
}

jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetKeyA(JNIEnv *env,
                                                                jobject thiz) {
    char ciphertext_0[128] = {0};
    mLicence->getKeyA(ciphertext_0);
    return env->NewStringUTF(ciphertext_0);
}

jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetKeyH(JNIEnv *env,
                                                                jobject thiz,
                                                                jstring id) {
    jboolean isCopy;
    const char *str = env->GetStringUTFChars(id, &isCopy);
    char ciphertext_0[128] = {0};
    mLicence->getKeyH(str, ciphertext_0);
    return env->NewStringUTF(ciphertext_0);
}

jboolean JNICALL Java_com_wq_player_ndk_NdkLicence_nativeIsAllow(JNIEnv *env,
                                                                 jobject thiz,
                                                                 jstring hardId,
                                                                 jstring result1,
                                                                 jstring result2) {
    jboolean isCopy;
    const char *hId = env->GetStringUTFChars(hardId, &isCopy);
    const char *r1 = env->GetStringUTFChars(result1, &isCopy);
    const char *r2 = env->GetStringUTFChars(result2, &isCopy);
    bHaveLicence = mLicence->isAllow(hId, r1, r2);
    return bHaveLicence;
}
