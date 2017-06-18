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

    void setTransformBean(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat fov, GLfloat scale,
                          GLfloat lookAtCenterZ);

    SettingsBean *getSettingsBean();

    TransformBean *getTransformBean();

    void anim(TransformBean *from, TransformBean *to, GLuint during);

    void sleep(GLuint ms);

    void set(TransformBean *bean, GLfloat x, GLfloat y, GLfloat z, GLfloat fov, GLfloat scale,
             GLfloat centerZ);

private:
    SettingsBean *mSettingsBean;
    TransformBean *mTransformBean;

    struct timeval now;
    struct timespec outtime;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

#endif //BEAN_H
