#ifndef REGION_H
#define REGION_H

#include <jni.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include "bean/bean_base.h"
#include "log.h"

typedef struct {
    GLfloat maxSetUpValue;  // 设定值
    GLfloat minSetUpValue;
    GLfloat maxActualValue; // 实际值(实际旋转角度受FOV限制)
    GLfloat minActualValue;
    GLfloat defaultValue;
    GLfloat value;
} _Region;

class Region {
public:
    _Region *degreeX;
    _Region *degreeY;
    _Region *degreeZ;
    _Region *fov;

    Region();

    ~Region();

    void setDefaultX(GLfloat min, GLfloat max, GLfloat def);

    void setDefaultY(GLfloat min, GLfloat max, GLfloat def);

    void setDefaultZ(GLfloat min, GLfloat max, GLfloat def);

    void setDefaultFov(GLfloat min, GLfloat max, GLfloat def);

    void limit(TransformBean *bean);

    void reset();

private:
    void setFov(GLfloat fov, GLfloat ratio);
    void setDegreeX(GLfloat x);
    void setDegreeY(GLfloat y);
    void setDegreeZ(GLfloat z);
};

#endif //REGION_H
