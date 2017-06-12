#ifndef BEAN_H
#define BEAN_H

#include <GLES3/gl3.h>
#include "transform/transform.h"
#include "bean/bean_base.h"
#include "gl/gl_renderer.h"
#include "log.h"

class Bean {
public:
    Bean(SettingsBean bean);

    ~Bean();

    void setSettingsBean(SettingsBean bean);

    void setSettingsBean(GLRenderer *renderer, Transform *transform, SettingsBean bean);

    void setTransformBean(TransformBean bean);

    void setTransformBean(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat fov, GLfloat scale);

    SettingsBean *getSettingsBean();

    TransformBean *getTransformBean();

private:
    SettingsBean *mSettingsBean;
    TransformBean *mTransformBean;
};

#endif //BEAN_H
