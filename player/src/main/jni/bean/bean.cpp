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
    GLfloat stepScale = (to->scale - from->scale) / stepCount;
    GLfloat stepX = (to->degreeX - from->degreeX) / stepCount;
    GLfloat stepY = (to->degreeY - from->degreeY) / stepCount;
    GLfloat stepZ = (to->degreeZ - from->degreeZ) / stepCount;
    GLfloat stepFov = (to->fov - from->fov) / stepCount;

    for (int i = 0; i < stepCount; i++) {
        from->fov       += stepFov;
        from->scale     += stepScale;
        from->degreeX   += stepX;
        from->degreeY   += stepY;
        from->degreeZ   += stepZ;

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