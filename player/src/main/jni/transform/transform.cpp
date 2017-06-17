#include <bean/bean_base.h>
#include "transform/transform.h"

Transform::Transform(TransformBean *transformBean, SettingsBean *settingsBean) {
    mTransformBean = transformBean;
    mSettingsBean = settingsBean;
    mTouch = new Touch(transformBean, settingsBean);
    mSensor = new Sensor(settingsBean);
    mRegion = new Region();
    setDefaultRegion(mSettingsBean->mShowMode);
}

Transform::~Transform() {
    delete mTouch;
    delete mSensor;
    delete mRegion;
}

void Transform::setDefaultRegion(GLuint sm) {
//    mRegion->setDefaultFov(FOV_MIN, FOV_MAX, FOV_DEFAULT);
    switch (sm) {
        case SM_NORMAL:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            mRegion->setDefaultFov(60, 120, FOV_DEFAULT);
            mRegion->setDefaultScale(0.6, 2.0, 1);
            break;
        case SM_ASTEROID:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            mRegion->setDefaultFov(FOV_ASTEROID, FOV_ASTEROID + 0.01f, FOV_ASTEROID);
            mRegion->setDefaultScale(0.6, 2.0, 1);
            break;
        case SM_SPHERE:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            mRegion->setDefaultFov(FOV_DEFAULT, FOV_DEFAULT + 0.01f, FOV_DEFAULT);
            mRegion->setDefaultScale(0.6, 2.0, 1);
            break;
        case SM_OTHER:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            mRegion->setDefaultFov(30, 150, FOV_DEFAULT);
            mRegion->setDefaultScale(0.6, 2.0, 1);
            break;
        default:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            mRegion->setDefaultFov(0, 0, FOV_DEFAULT);
            mRegion->setDefaultScale(0.6, 2.0, 1);
            break;
    }
}

void Transform::limit(TransformBean *transformBean) {
    mRegion->limit(transformBean);
}

GLboolean Transform::onTouch(GLuint action, GLuint pointerCount,
                             GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    TransformBean *bean = new TransformBean();
    bean->fov = mTransformBean->fov;
    bean->degreeX = mTransformBean->degreeX;
    bean->degreeY = mTransformBean->degreeY;
    bean->degreeZ = mTransformBean->degreeZ;
    bean->scale = mTransformBean->scale;
    GLboolean result = mTouch->onTouch(bean, action, pointerCount, x1, y1, x2, y2);
    limit(bean);
    mTransformBean->fov = bean->fov;
    mTransformBean->degreeX = bean->degreeX;
    mTransformBean->degreeY = bean->degreeY;
    mTransformBean->degreeZ = bean->degreeZ;
    mTransformBean->scale = bean->scale;
    delete bean;
    return result;
}

void Transform::onSensor(GLfloat x, GLfloat y, GLfloat z, GLuint64 timestamp) {
    TransformBean *bean = new TransformBean();
    bean->fov = mTransformBean->fov;
    bean->degreeX = mTransformBean->degreeX;
    bean->degreeY = mTransformBean->degreeY;
    bean->degreeZ = mTransformBean->degreeZ;
    mSensor->onSensor(bean, x, y, z, timestamp);
    limit(bean);
    mTransformBean->fov = bean->fov;
    mTransformBean->degreeX = bean->degreeX;
    mTransformBean->degreeY = bean->degreeY;
    mTransformBean->degreeZ = bean->degreeZ;
    delete bean;
}

void Transform::reset() {
    mRegion->reset();
    mTransformBean->fov = mRegion->fov->value;
    mTransformBean->scale = mRegion->scale->value;
    mTransformBean->degreeX = mRegion->degreeX->value;
    mTransformBean->degreeY = mRegion->degreeY->value;
    mTransformBean->degreeZ = mRegion->degreeZ->value;
}

GLboolean Transform::onSettingsChanged(GLuint last_sm, GLuint last_rr, GLuint last_cs) {
    GLboolean result = GL_FALSE;
    if (last_sm != mSettingsBean->mShowMode) {
        LOGI("[Transform:onSettingsChanged]mShowMode is changed");
        setDefaultRegion(mSettingsBean->mShowMode);
        result = GL_TRUE;
    }
    if (last_rr != mSettingsBean->mResolutionRatio) {
        LOGI("[Transform:onSettingsChanged]mResolutionRatio is changed");
    }
    if (last_cs != mSettingsBean->mCtrlStyle) {
        LOGI("[Transform:onSettingsChanged]mCtrlStyle is changed");
    }
    reset();
    return result;
}