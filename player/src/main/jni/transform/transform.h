#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <GLES3/gl3.h>
#include <math.h>
#include <stdlib.h>
#include "bean/bean_base.h"
#include "bean/region.h"
#include "transform/touch.h"
#include "transform/sensor.h"
#include "log.h"

class Transform {
public:
    Transform(TransformBean *curr, TransformBean *next, SettingsBean *settingsBean);

    ~Transform();

    GLboolean onTouch(GLuint action, GLuint pointerCount,
                      GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void onSensor(GLfloat x, GLfloat y, GLfloat z, GLuint64 timestamp);

    GLboolean onSettingsChanged(GLuint last_sm, GLuint last_rr, GLuint last_cs);

    void reset();

    void reset(GLboolean anim);

    void setDefaultRegion(GLuint sm);

    void limit(TransformBean *transformBean);

private:
    TransformBean *mTransformBean;
    TransformBean *mNextTransformBean;
    SettingsBean *mSettingsBean;
    Region *mRegion;
    Touch *mTouch;
    Sensor *mSensor;
};

#endif //TRANSFORM_H
