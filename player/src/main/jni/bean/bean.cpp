#include "bean/bean.h"
#include "bean_base.h"

Bean::Bean(SettingsBean bean) {
    mSettingsBean = new SettingsBean();
    mTransformBean = new TransformBean();

    setTransformBean(0, 0, 0, FOV_DEFAULT, 1);
    setSettingsBean(bean);
}

Bean::~Bean() {
    delete mSettingsBean;
    delete mTransformBean;
}

template<class T, typename func, typename P1, typename P2, typename P3>
GLboolean onSettingsChanged(T *pObj, func p, P1 sm, P2 rr, P3 cs) {
    return (pObj->*p)(sm, rr, cs);
}

void Bean::setSettingsBean(GLRenderer *renderer, Transform *transform, SettingsBean bean) {
    LOGI("[bean:setSettingsBean:cb]last bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);

    GLuint lastSM = mSettingsBean->mShowMode;
    GLuint lastRR = mSettingsBean->mResolutionRatio;
    GLuint lastCS = mSettingsBean->mCtrlStyle;

    mSettingsBean->isUseBitmap = bean.isUseBitmap;
    mSettingsBean->isLeft = bean.isLeft;
    mSettingsBean->mShowMode = bean.mShowMode;
    mSettingsBean->mCtrlStyle = bean.mCtrlStyle;
    mSettingsBean->mResolutionRatio = bean.mResolutionRatio;
    mSettingsBean->mAppPath = bean.mAppPath;

    onSettingsChanged(renderer, &GLRenderer::onSettingsChanged, lastSM, lastRR, lastCS);
    onSettingsChanged(transform, &Transform::onSettingsChanged, lastSM, lastRR, lastCS);

    LOGI("[bean:setSettingsBean:cb]curr bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);
}

void Bean::setSettingsBean(SettingsBean bean) {
    LOGI("[bean:setSettingsBean]last bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);

    mSettingsBean->isUseBitmap = bean.isUseBitmap;
    mSettingsBean->isLeft = bean.isLeft;
    mSettingsBean->mShowMode = bean.mShowMode;
    mSettingsBean->mCtrlStyle = bean.mCtrlStyle;
    mSettingsBean->mResolutionRatio = bean.mResolutionRatio;
    mSettingsBean->mAppPath = bean.mAppPath;

    LOGI("[bean:setSettingsBean]curr bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);
}

void Bean::setTransformBean(TransformBean bean) {
    LOGI("[bean:setTransformBean]last bean degree(%f, %f, %f), zoom=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov);
    if (mTransformBean->degreeX != bean.degreeX) {
        mTransformBean->degreeX = bean.degreeX;
    }

    if (mTransformBean->degreeY != bean.degreeY) {
        mTransformBean->degreeY = bean.degreeY;
    }
    if (mTransformBean->degreeZ != bean.degreeZ) {
        mTransformBean->degreeZ = bean.degreeZ;
    }
    if (mTransformBean->fov != bean.fov) {
        mTransformBean->fov = bean.fov;
    }
    if (mTransformBean->scale != bean.scale) {
        mTransformBean->scale = bean.scale;
    }
    LOGI("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f, scale=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov, mTransformBean->scale);
}

void Bean::setTransformBean(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat fov, GLfloat scale) {
    LOGI("[bean:setTransformBean]last bean degree(%f, %f, %f), zoom=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov);
    if (mTransformBean->degreeX != rx) {
        mTransformBean->degreeX = rx;
    }

    if (mTransformBean->degreeY != ry) {
        mTransformBean->degreeY = ry;
    }
    if (mTransformBean->degreeZ != rz) {
        mTransformBean->degreeZ = rz;
    }
    if (mTransformBean->fov != fov) {
        mTransformBean->fov = fov;
    }
    if (mTransformBean->scale != scale) {
        mTransformBean->scale = scale;
    }
    LOGI("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f, scale=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov, mTransformBean->scale);
}

SettingsBean *Bean::getSettingsBean() {
    return mSettingsBean;
}

TransformBean *Bean::getTransformBean() {
    return mTransformBean;
}

void Bean::anim(TransformBean *from, TransformBean *to, GLuint during) {
    GLuint timeBetweenFrame = 40;
    GLuint stepCount = (GLuint) (1.0f * during / timeBetweenFrame);
    GLuint startCount = stepCount / 10;
    GLuint endCount = stepCount / 10;
    GLuint midCount = stepCount - startCount - endCount;
    GLfloat midStepScale = (to->scale - from->scale) / stepCount;
    GLfloat midStepX = (to->degreeX - from->degreeX) / stepCount;
    GLfloat midStepY = (to->degreeY - from->degreeY) / stepCount;
    GLfloat midStepZ = (to->degreeZ - from->degreeZ) / stepCount;
    GLfloat midStepFov = (to->fov - from->fov) / stepCount;

    LOGE("qqqq from:%f, %f, %f; to:%f, %f, %f", from->degreeX, from->degreeY, from->fov,
         to->degreeX, to->degreeY, to->fov);
    // 单帧变化值
    GLfloat stepScale = 0;
    GLfloat stepX = 0;
    GLfloat stepY = 0;
    GLfloat stepZ = 0;
    GLfloat stepFov = 0;

    // 单帧变化值的加速度
    // 加速-匀速-减速
    // 1 加速
    GLfloat aScale = midStepScale / startCount;
    GLfloat aX = midStepX / startCount;
    GLfloat aY = midStepY / startCount;
    GLfloat aZ = midStepZ / startCount;
    GLfloat aFov = midStepFov / startCount;

    for (int i = 0; i < startCount; i++) {
        stepScale += aScale;
        stepX += aX;
        stepY += aY;
        stepZ += aZ;
        stepFov += aFov;

        from->fov       += stepFov;
        from->scale     += stepScale;
        from->degreeX   += stepX;
        from->degreeY   += stepY;
        from->degreeZ   += stepZ;
        LOGE("qqqq aaaa %f, %f, %f; step:%f, %f, %f", from->degreeX, from->degreeY, from->fov,
             stepX, stepY, stepFov);
        sleep(timeBetweenFrame);
    }

    // 2 匀速
    for (int i = 0; i < midCount; i++) {
        from->fov       += stepFov;
        from->scale     += stepScale;
        from->degreeX   += stepX;
        from->degreeY   += stepY;
        from->degreeZ   += stepZ;

        LOGE("qqqq bbbb %f, %f, %f; step:%f, %f, %f", from->degreeX, from->degreeY, from->fov,
             stepX, stepY, stepFov);
        sleep(timeBetweenFrame);
    }

    // 3 减速
    aScale = stepScale * stepScale / (to->scale - from->scale) / 2;
    aX = stepX * stepX / (to->degreeX - from->degreeX) / 2;
    aY = stepY * stepY / (to->degreeY - from->degreeY) / 2;
    aZ = stepZ * stepZ / (to->degreeZ - from->degreeZ) / 2;
    aFov = stepFov * stepFov / (to->fov - from->fov) / 2;

    while(  fabsf(stepFov) > 0.01 || fabsf(stepScale) > 0.01 ||
            fabsf(stepX) > 0.01 || fabsf(stepY) > 0.01 || fabsf(stepZ) > 0.01) {
        stepScale -= aScale;
        stepX -= aX;
        stepY -= aY;
        stepZ -= aZ;
        stepFov -= aFov;

        from->fov       += stepFov;
        from->scale     += stepScale;
        from->degreeX   += stepX;
        from->degreeY   += stepY;
        from->degreeZ   += stepZ;

        LOGE("qqqq cccc %f, %f, %f; step:%f, %f, %f", from->degreeX, from->degreeY, from->fov,
             stepX, stepY, stepFov);
        sleep(timeBetweenFrame);
    }
}

void Bean::sleep(GLuint ms) {
    gettimeofday(&now, NULL);
    now.tv_usec += 1000 * ms;
    if (now.tv_usec > 1000000) {
        now.tv_sec += now.tv_usec / 1000000;
        now.tv_usec %= 1000000;
    }

    outtime.tv_sec = now.tv_sec;
    outtime.tv_nsec = now.tv_usec * 1000;

    pthread_cond_timedwait(&cond, &mutex, &outtime);
}

void Bean::set(TransformBean *bean, GLfloat x, GLfloat y, GLfloat z, GLfloat fov, GLfloat scale) {
    if(x != -1) bean->degreeX = x;
    if(y != -1) bean->degreeY = y;
    if(z != -1) bean->degreeZ = z;
    if(fov != -1) bean->fov = fov;
    if(scale != -1) bean->scale = scale;
}