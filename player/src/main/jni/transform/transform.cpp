#include "transform/transform.h"

Transform::Transform(TransformBean *transformBean, SettingsBean *settingsBean) {
    mTransformBean = transformBean;
    mSettingsBean = settingsBean;
    mTouch = new Touch(settingsBean);
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
    mRegion->setDefaultFov(FOV_MIN, FOV_MIN, FOV_DEFAULT);
    switch (sm){
        case SM_ORIGINAL:
            mRegion->setDefaultX(-45, 45, 0);
            mRegion->setDefaultY(-45, 45, 0);
            mRegion->setDefaultZ(0, 0, 0);
            break;
        case SM_SPHERE_FRONT:
            mRegion->setDefaultX(-90, 90, 0);
            mRegion->setDefaultY(-90, 90, 0);
            mRegion->setDefaultZ(0, 0, 0);
            break;
        case SM_SPHERE_FRONT_BACK:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            break;
        case SM_SPHERE_UP:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            break;
        default:
            mRegion->setDefaultX(0, 0, 0);
            mRegion->setDefaultY(0, 0, 0);
            mRegion->setDefaultZ(0, 0, 0);
            break;
    }
    reset();
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
    GLboolean result = mTouch->onTouch(bean, action, pointerCount, x1, y1, x2, y2);
    limit(bean);
    mTransformBean->fov = bean->fov;
    mTransformBean->degreeX = bean->degreeX;
    mTransformBean->degreeY = bean->degreeY;
    mTransformBean->degreeZ = bean->degreeZ;
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
    mTransformBean->degreeX = mRegion->degreeX->value;
    mTransformBean->degreeY = mRegion->degreeY->value;
    mTransformBean->degreeZ = mRegion->degreeZ->value;
}

GLboolean Transform::onSettingsChanged(GLuint sm, GLuint rr, GLuint cs) {
    GLboolean result = GL_FALSE;
    if (sm != mSettingsBean->mShowMode) {
        LOGD("[Transform:onSettingsChanged]mShowMode is changed");
        setDefaultRegion(mSettingsBean->mShowMode);
        result = GL_TRUE;
    }
    if (rr != mSettingsBean->mResolutionRatio) {
        LOGD("[Transform:onSettingsChanged]mResolutionRatio is changed");
    }
    if (cs != mSettingsBean->mCtrlStyle) {
        LOGD("[Transform:onSettingsChanged]mCtrlStyle is changed");
    }
    return result;
}