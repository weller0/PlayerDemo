#ifndef TOUCH_H
#define TOUCH_H

#include <GLES3/gl3.h>
#include <cmath>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "bean/bean_base.h"
#include "log.h"

using namespace std;

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

static const GLfloat MIN = 0.2f;
static const GLfloat DEGREE_PER_1000PX = 1.2f;

class Touch {
public:
    Touch(TransformBean *transformBean, SettingsBean *settingsBean);

    ~Touch();

    GLboolean onTouch(TransformBean *bean, GLuint action, GLuint pointerCount,
                      GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void sleep(GLuint ms);

    GLboolean bExitThread;
    GLfloat mDeltaX, mDeltaY;
    GLfloat step;
    Point2 *mStartPoint;
    Point2 *mLastPoint;
    TransformBean *mTransformBean;
    pthread_t pThreadForAnim;

private:
    SettingsBean *mSettingsBean;
    GLfloat mDistance;
    Point2 *mMidPoint;
    GLuint mMode;
    GLuint64 mLastTime;
    struct timeval now;
    struct timespec outtime;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    GLuint64 mPressTime;

    void distance(GLfloat *dis, GLuint pointerCount,
                  GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void midPoint(Point2 *midPoint, GLuint pointerCount,
                  GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    void setDrag(TransformBean *bean, GLfloat x, GLfloat y, GLuint time);

    void setZoom(TransformBean *bean, GLuint pointerCount,
                 GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    GLuint64 getCurrentTime();

    static void *thread_fun(void *arg);

    void fling(GLfloat vx, GLfloat vy);

    void anim();
};

#endif //TOUCH_H
