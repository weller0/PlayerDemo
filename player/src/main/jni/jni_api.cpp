#include <bean/bean_base.h>
#include "jni_api.h"

extern "C" {
GLRenderer *pGLDisplay = NULL;
Transform *mTransform = NULL;
Bean *mBean = NULL;

jboolean bHaveLicence = JNI_FALSE;

pthread_t pThreadForCircle;
jboolean bExitThread;
struct timeval now;
struct timespec outtime;
pthread_cond_t cond;
pthread_mutex_t mutex;

void sleep(GLuint ms) {
    gettimeofday(&now, NULL);
    now.tv_usec += 1000*ms;
    if (now.tv_usec > 1000000) {
        now.tv_sec += now.tv_usec / 1000000;
        now.tv_usec %= 1000000;
    }

    outtime.tv_sec = now.tv_sec;
    outtime.tv_nsec = now.tv_usec * 1000;

    pthread_cond_timedwait(&cond, &mutex, &outtime);
}

void* thread_fun(void *arg) {
    sleep(2000);
    float deltaX = 1;
    while(!bExitThread) {
        mBean->getTransformBean()->degreeX += deltaX;
        if(mBean->getTransformBean()->degreeX >= 336){
            deltaX -= 0.02;
            if(deltaX < 0 || mBean->getTransformBean()->degreeX >= 360) {
                mBean->getTransformBean()->degreeX = 0;
                bExitThread = GL_TRUE;
            }
        }
        sleep(20);
    }

    pthread_kill(pThreadForCircle, 0);

    return NULL;
}

void anim() {
    if(pThreadForCircle != 0) {
        pthread_detach(pThreadForCircle);
        pThreadForCircle = 0;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    bExitThread = GL_FALSE;
    pthread_create(&pThreadForCircle, NULL, thread_fun, NULL);
}

SettingsBean cpp2JavaForSettingsBean(JNIEnv *env, jobject bean) {
    SettingsBean settingsBean;
    settingsBean.isUseBitmap = (GLboolean) env->CallBooleanMethod(bean, midIsUseBitmap);
    settingsBean.isLeft = (GLboolean) env->CallBooleanMethod(bean, midIsLeft);
    settingsBean.mShowMode = (GLuint) env->CallIntMethod(bean, midShowMode);
    settingsBean.mCtrlStyle = (GLuint) env->CallIntMethod(bean, midCtrlStyle);
    settingsBean.mResolutionRatio = (GLuint) env->CallIntMethod(bean, midResolutionRatio);
    jstring path = (jstring) env->CallObjectMethod(bean, midAppPath);
    jboolean isCopy;
    const char *str = env->GetStringUTFChars(path, &isCopy);
    int len = strlen(str);
    settingsBean.mAppPath = (char *) malloc((len + 1) * sizeof(char));
    memset(settingsBean.mAppPath, 0, (len + 1));
    memcpy(settingsBean.mAppPath, str, len * sizeof(char));
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
                                                                 jobject bmp,
                                                                 jboolean updateFrameData) {
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
            pGLDisplay->onDrawFrame(bitmap, updateFrameData);
            delete bitmap;
        } else {
            pGLDisplay->onDrawFrame(NULL, updateFrameData);
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
    midAppPath = env->GetMethodID(clsSettingsBean, "getAppPath", "()Ljava/lang/String;");
    mBean = new Bean(cpp2JavaForSettingsBean(env, bean));
    mTransform = new Transform(mBean->getTransformBean(), mBean->getSettingsBean());
    if (mBean->getSettingsBean()->isUseBitmap) {
        pGLDisplay = new Picture(mBean->getTransformBean(), mBean->getSettingsBean());
    } else {
        pGLDisplay = new PlayYuv(mBean->getTransformBean(), mBean->getSettingsBean());
    }

    if(mBean->getSettingsBean()->mCtrlStyle == CS_DRAG ||
            mBean->getSettingsBean()->mCtrlStyle == CS_DRAG_ZOOM) {
        anim();
    }
}

void JNICALL Java_com_wq_player_ndk_NdkPicLeft_nativeReleaseApi(JNIEnv *env,
                                                                jobject obj) {
    LOGI("[jni_api]release");
    bHaveLicence = JNI_FALSE;
    bExitThread = JNI_TRUE;
    if(pThreadForCircle != 0) {
        pthread_detach(pThreadForCircle);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
    if (pGLDisplay != NULL) {
        delete pGLDisplay;
    }
    if (mTransform != NULL) {
        delete mTransform;
    }
    if (mBean != NULL) {
        delete mBean;
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
    if(bExitThread) {
        mTransform->onTouch(action, pointCount, x1, y1, x2, y2);
    }
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

jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetEncodeA(JNIEnv *env,
                                                                   jobject thiz) {
    char ciphertext_0[128] = {0};
    Licence *licence = new Licence();
    licence->getEncodeA(ciphertext_0);
    delete licence;
    return env->NewStringUTF(ciphertext_0);
}

jstring JNICALL Java_com_wq_player_ndk_NdkLicence_nativeGetEncodeH(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jstring id) {
    jboolean isCopy;
    const char *str = env->GetStringUTFChars(id, &isCopy);
    char ciphertext_0[128] = {0};
    Licence *licence = new Licence();
    licence->encode(str, ciphertext_0);
    delete licence;
    return env->NewStringUTF(ciphertext_0);
}

jboolean JNICALL Java_com_wq_player_ndk_NdkLicence_nativeIsAllow(JNIEnv *env,
                                                                 jobject thiz,
                                                                 jstring hardId,
                                                                 jstring result1,
                                                                 jstring result2) {
    if(hardId != NULL && result1 != NULL && result2 !=NULL){
        jboolean isCopy;
        const char *hId = env->GetStringUTFChars(hardId, &isCopy);
        const char *r1 = env->GetStringUTFChars(result1, &isCopy);
        const char *r2 = env->GetStringUTFChars(result2, &isCopy);
        Licence *licence = new Licence();
        bHaveLicence = licence->isAllow(hId, r1, r2);
        delete licence;
    } else {
        bHaveLicence = JNI_FALSE;
    }
    bHaveLicence = JNI_TRUE;
    return bHaveLicence;
}

jboolean JNICALL Java_com_wq_player_ndk_NdkLicence_nativeHasLicence(JNIEnv *env,
                                                                    jobject thiz) {
    return bHaveLicence;
}
}
