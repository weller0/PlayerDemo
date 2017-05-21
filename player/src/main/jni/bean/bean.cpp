#include "bean/bean.h"

Bean::Bean(SettingsBean bean) {
    mSettingsBean = new SettingsBean();
    mTransformBean = new TransformBean();

    setTransformBean(0, 0, 0, FOV_DEFAULT);
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
    LOGI("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov);
}

void Bean::setTransformBean(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat fov) {
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
    LOGI("[bean:setTransformBean]curr bean degree(%f, %f, %f), zoom=%f",
         mTransformBean->degreeX, mTransformBean->degreeY,
         mTransformBean->degreeZ, mTransformBean->fov);
}

SettingsBean *Bean::getSettingsBean() {
    return mSettingsBean;
}

TransformBean *Bean::getTransformBean() {
    return mTransformBean;
}