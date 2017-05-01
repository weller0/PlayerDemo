#ifndef TOUCH_H
#define TOUCH_H

#include <GLES3/gl3.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include "bean/bean_base.h"
#include "log.h"

static const GLuint ACTION_DOWN = 0;
static const GLuint ACTION_UP = 1;
static const GLuint ACTION_MOVE = 2;
static const GLuint ACTION_CANCEL = 3;
static const GLuint ACTION_OUTSIDE = 4;
static const GLuint ACTION_POINTER_DOWN = 5;
static const GLuint ACTION_POINTER_UP = 6;

static const GLuint MODE_NORMAL = 1;
static const GLuint MODE_DRAG = 2;
static const GLuint MODE_ZOOM = 3;

class Touch {
public:
    Touch(SettingsBean *settingsBean);

    ~Touch();

    GLboolean onTouch(TransformBean *bean, GLuint action, GLuint pointerCount,
                      GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

private:
    SettingsBean *mSettingsBean;
    GLfloat mDistance;
    Point2 *mStartPoint;
    Point2 *mMidPoint;
    GLuint mMode;
    GLuint64 mLastTime;

    void distance(GLfloat *dis, GLuint pointerCount,
                  GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void midPoint(Point2 *midPoint, GLuint pointerCount,
                  GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void setDrag(TransformBean *bean, GLfloat x, GLfloat y, GLuint time);

    void setZoom(TransformBean *bean, GLuint pointerCount,
                 GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    GLuint64 getCurrentTime();
};

#endif //TOUCH_H
