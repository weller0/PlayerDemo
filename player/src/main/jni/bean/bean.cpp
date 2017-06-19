#include "bean/bean.h"

Bean::Bean(SettingsBean bean) {
    mSettingsBean = new SettingsBean();
    mTransformBean = new TransformBean();
    mNextTransformBean = new TransformBean();

    set(mNextTransformBean, 0, 0, 0, FOV_DEFAULT, 1, 0);
    set(mTransformBean, -90, 90, 0, FOV_ASTEROID, 1, 0);
    setSettingsBean(bean);
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
}

Bean::~Bean() {
    delete mSettingsBean;
    delete mTransformBean;
    delete mNextTransformBean;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
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
    LOGD("[bean:setSettingsBean]last bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);

    mSettingsBean->isUseBitmap = bean.isUseBitmap;
    mSettingsBean->isLeft = bean.isLeft;
    mSettingsBean->mShowMode = bean.mShowMode;
    mSettingsBean->mCtrlStyle = bean.mCtrlStyle;
    mSettingsBean->mResolutionRatio = bean.mResolutionRatio;
    mSettingsBean->mAppPath = bean.mAppPath;

    LOGD("[bean:setSettingsBean]curr bean isUseBitmap:%d, left:%d, sm:%d, rr:%d, cs:%d",
         mSettingsBean->isLeft, mSettingsBean->isUseBitmap, mSettingsBean->mShowMode,
         mSettingsBean->mResolutionRatio, mSettingsBean->mCtrlStyle);
}

void Bean::setTransformBean(TransformBean bean) {
    LOGD("[bean:setTransformBean]last bean degree(%f, %f, %f), zoom=%f",
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
    if (mTransformBean->lookAtCenterZ != bean.lookAtCenterZ) {
        mTransformBean->lookAtCenterZ = bean.lookAtCenterZ;
    }
    LOGD("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f, scale=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov, mTransformBean->scale);
}

void Bean::setTransformBean(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat fov, GLfloat scale,
                            GLfloat lookAtCenterZ) {
    LOGD("[bean:setTransformBean]last bean degree(%f, %f, %f), zoom=%f",
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
    if (mTransformBean->lookAtCenterZ != lookAtCenterZ) {
        mTransformBean->lookAtCenterZ = lookAtCenterZ;
    }
    LOGD("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f, scale=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov, mTransformBean->scale);
}

SettingsBean *Bean::getSettingsBean() {
    return mSettingsBean;
}

TransformBean *Bean::getTransformBean() {
    return mTransformBean;
}

TransformBean *Bean::getNextTransformBean() {
    return mNextTransformBean;
}

void Bean::anim(TransformBean *from, TransformBean *to, GLuint during) {
    if (mSettingsBean->mShowMode != SM_SPHERE) {
        to->autoChangeCenterZ();
    }
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
    GLfloat midStepCenterZ = (to->lookAtCenterZ - from->lookAtCenterZ) / stepCount;

    // 单帧变化值
    GLfloat stepScale = 0;
    GLfloat stepX = 0;
    GLfloat stepY = 0;
    GLfloat stepZ = 0;
    GLfloat stepFov = 0;
    GLfloat stepCenterZ = 0;

    // 单帧变化值的加速度
    // 加速-匀速-减速
    // 1 加速
    GLfloat aScale = midStepScale / startCount;
    GLfloat aX = midStepX / startCount;
    GLfloat aY = midStepY / startCount;
    GLfloat aZ = midStepZ / startCount;
    GLfloat aFov = midStepFov / startCount;
    GLfloat aCenterZ = midStepCenterZ / startCount;

    for (int i = 0; i < startCount; i++) {
        stepScale += aScale;
        stepX += aX;
        stepY += aY;
        stepZ += aZ;
        stepFov += aFov;
        stepCenterZ += aCenterZ;

        from->fov += stepFov;
        from->scale += stepScale;
        from->degreeX += stepX;
        from->degreeY += stepY;
        from->degreeZ += stepZ;
        from->lookAtCenterZ += stepCenterZ;
        sleep(timeBetweenFrame);
    }

    // 2 匀速
    for (int i = 0; i < midCount; i++) {
        from->fov += stepFov;
        from->scale += stepScale;
        from->degreeX += stepX;
        from->degreeY += stepY;
        from->degreeZ += stepZ;
        from->lookAtCenterZ += stepCenterZ;
        sleep(timeBetweenFrame);
    }

    // 3 减速
    if (to->scale - from->scale == 0) {
        aScale = 0;
    } else {
        aScale = stepScale * stepScale / (to->scale - from->scale) / 2;
    }
    if (to->degreeX - from->degreeX == 0) {
        aX = 0;
    } else {
        aX = stepX * stepX / (to->degreeX - from->degreeX) / 2;
    }
    if (to->degreeY - from->degreeY == 0) {
        aY = 0;
    } else {
        aY = stepY * stepY / (to->degreeY - from->degreeY) / 2;
    }
    if (to->degreeZ - from->degreeZ == 0) {
        aZ = 0;
    } else {
        aZ = stepZ * stepZ / (to->degreeZ - from->degreeZ) / 2;
    }
    if (to->fov - from->fov == 0) {
        aFov = 0;
    } else {
        aFov = stepFov * stepFov / (to->fov - from->fov) / 2;
    }
    if (to->lookAtCenterZ - from->lookAtCenterZ == 0) {
        aCenterZ = 0;
    } else {
        aCenterZ = stepCenterZ * stepCenterZ / (to->lookAtCenterZ - from->lookAtCenterZ) / 2;
    }

    while (fabsf(stepFov) > fabsf(aFov * 8) || fabsf(stepScale) > fabsf(aScale * 8) ||
           fabsf(stepX) > fabsf(aX * 8) || fabsf(stepY) > fabsf(aY * 8) ||
           fabsf(stepZ) > fabsf(aZ * 8) || fabsf(stepCenterZ) > fabsf(aCenterZ * 8)) {
        stepScale -= aScale;
        stepX -= aX;
        stepY -= aY;
        stepZ -= aZ;
        stepFov -= aFov;
        stepCenterZ -= aCenterZ;

        from->fov += stepFov;
        from->scale += stepScale;
        from->degreeX += stepX;
        from->degreeY += stepY;
        from->degreeZ += stepZ;
        from->lookAtCenterZ += stepCenterZ;
        sleep(timeBetweenFrame);
    }

    // 4 减速直到停下来
    if (to->scale - from->scale == 0) {
        aScale = 0;
    } else {
        aScale = stepScale * stepScale / (to->scale - from->scale) / 2;
    }
    if (to->degreeX - from->degreeX == 0) {
        aX = 0;
    } else {
        aX = stepX * stepX / (to->degreeX - from->degreeX) / 2;
    }
    if (to->degreeY - from->degreeY == 0) {
        aY = 0;
    } else {
        aY = stepY * stepY / (to->degreeY - from->degreeY) / 2;
    }
    if (to->degreeZ - from->degreeZ == 0) {
        aZ = 0;
    } else {
        aZ = stepZ * stepZ / (to->degreeZ - from->degreeZ) / 2;
    }
    if (to->fov - from->fov == 0) {
        aFov = 0;
    } else {
        aFov = stepFov * stepFov / (to->fov - from->fov) / 2;
    }
    if (to->lookAtCenterZ - from->lookAtCenterZ == 0) {
        aCenterZ = 0;
    } else {
        aCenterZ = stepCenterZ * stepCenterZ / (to->lookAtCenterZ - from->lookAtCenterZ) / 2;
    }

    while (fabsf(stepFov) > fabsf(aFov * 0.6f) || fabsf(stepScale) > fabsf(aScale * 0.6f) ||
           fabsf(stepX) > fabsf(aX * 0.6f) || fabsf(stepY) > fabsf(aY * 0.6f) ||
           fabsf(stepZ) > fabsf(aZ * 0.6f) || fabsf(stepCenterZ) > fabsf(aCenterZ * 0.6f)) {
        stepScale -= aScale;
        stepX -= aX;
        stepY -= aY;
        stepZ -= aZ;
        stepFov -= aFov;
        stepCenterZ -= aCenterZ;

        from->fov += stepFov;
        from->scale += stepScale;
        from->degreeX += stepX;
        from->degreeY += stepY;
        from->degreeZ += stepZ;
        from->lookAtCenterZ += stepCenterZ;
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

void Bean::set(TransformBean *bean, GLfloat x, GLfloat y, GLfloat z, GLfloat fov, GLfloat scale,
               GLfloat centerZ) {
    if (x != -1) bean->degreeX = x;
    if (y != -1) bean->degreeY = y;
    if (z != -1) bean->degreeZ = z;
    if (fov != -1) bean->fov = fov;
    if (scale != -1) bean->scale = scale;
    if (mSettingsBean->mShowMode != SM_SPHERE) {
        bean->autoChangeCenterZ();
    } else {
        if (centerZ != -1) bean->lookAtCenterZ = centerZ;
    }
}

void Bean::set(TransformBean *from, TransformBean *to) {
    from->degreeX = to->degreeX;
    from->degreeY = to->degreeY;
    from->degreeZ = to->degreeZ;
    from->fov = to->fov;
    from->scale = to->scale;
    if (mSettingsBean->mShowMode != SM_SPHERE) {
        from->autoChangeCenterZ();
    } else {
        from->lookAtCenterZ = to->lookAtCenterZ;
    }
}