#include <bean/bean_base.h>
#include "transform/touch.h"

Touch::Touch(TransformBean *transformBean, SettingsBean *settingsBean) {
    mSettingsBean = settingsBean;
    mTransformBean = transformBean;
    mStartPoint = new Point2();
    mLastPoint = new Point2();
    mMidPoint = new Point2();
    mMode = MODE_NORMAL;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

Touch::~Touch() {
    pthread_detach(pThreadForAnim);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    delete mStartPoint;
    delete mLastPoint;
    delete mMidPoint;
}

GLboolean Touch::onTouch(TransformBean *bean, GLuint action, GLuint pointerCount,
                         GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    switch (action) {
        case ACTION_DOWN:
            // 仅支持单指拖拽
            if (pointerCount == 1 && (mSettingsBean->mCtrlStyle == CS_DRAG ||
                                      mSettingsBean->mCtrlStyle == CS_DRAG_ZOOM)) {
                mMode = MODE_DRAG;
                mStartPoint->x = x1;
                mStartPoint->y = y1;
                mLastPoint->x = x1;
                mLastPoint->y = y1;
                mLastTime = getCurrentTime();
                mPressTime = mLastTime;
            }
            break;
        case ACTION_POINTER_DOWN:
            // 仅支持两指缩放
            if (pointerCount == 2 && mSettingsBean->mCtrlStyle == CS_DRAG_ZOOM) {
                distance(&mDistance, pointerCount, x1, y1, x2, y2);
                // 按下时两指间距大于10,执行手机动态缩放
                if (mDistance > 10) {
                    mMode = MODE_ZOOM;
                    midPoint(mMidPoint, pointerCount, x1, y1, x2, y2);
                }
            } else {
                mMode = MODE_NORMAL;
            }
            break;
        case ACTION_CANCEL:
        case ACTION_UP:
        case ACTION_POINTER_UP: {
            GLuint64 tt = getCurrentTime();
            GLfloat vx = (x1 - mStartPoint->x) * 1000.0f / (tt - mPressTime);
            GLfloat vy = (y1 - mStartPoint->y) * 1000.0f / (tt - mPressTime);
            fling(-vx, vy);
            mMode = MODE_NORMAL;
            break;
        }
        case ACTION_MOVE:
            if (mMode == MODE_DRAG) {
                GLuint64 time = getCurrentTime();
                setDrag(bean, x1, y1, (GLuint) (time - mLastTime));
                mLastTime = time;
            } else if (mMode == MODE_ZOOM) {
                setZoom(bean, pointerCount, x1, y1, x2, y2);
            }
            break;
    }
    return GL_TRUE;
}

void Touch::setDrag(TransformBean *bean, GLfloat x, GLfloat y, GLuint time) {
    // 计算拖拽的距离
    float dx = x - mLastPoint->x;
    float dy = y - mLastPoint->y;
    mLastPoint->x = x;
    mLastPoint->y = y;
    dx = TO_DEGREES(dx * time / 13000.0) * sinf(TO_RADIANS(bean->fov));
    dy = TO_DEGREES(dy * time / 13000.0) * sinf(TO_RADIANS(bean->fov));
    bean->degreeX -= dx;
    bean->degreeY -= dy;
}

GLfloat scaleMin = 0.6;
GLfloat scaleMax = 1.9;
GLfloat stepMin = 0.01;
GLfloat stepMax = 0.05;
GLfloat getStep(GLfloat currScale){
    GLfloat result = stepMin - (scaleMin - currScale) * (stepMin - stepMax) / (scaleMin - scaleMax);
    LOGE("[touch:setZoom]currScale=%f, step=%f", currScale, result);
    return result;
}

void Touch::setZoom(TransformBean *bean, GLuint pointerCount,
                    GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    GLfloat dis = 0;
    distance(&dis, pointerCount, x1, y1, x2, y2);
    if (dis == mDistance) return;
    // 获取将要缩放到比例,一般是1左右
    GLfloat deltaFov = (dis - mDistance > 0 ? 1 : -1) * dis / mDistance;
    deltaFov = deltaFov * getStep(bean->scale);
    mDistance = dis;
//    bean->fov -= deltaFov;
    bean->scale += deltaFov;
}

void Touch::distance(GLfloat *dis, GLuint pointerCount, GLfloat x1, GLfloat y1, GLfloat x2,
                     GLfloat y2) {
    if (pointerCount == 2) {
        GLfloat dx = x2 - x1;
        GLfloat dy = y2 - y1;
        *dis = sqrtf(dx * dx + dy * dy);
    } else {
        *dis = 0;
        LOGE("[touch:distance]Count of pointer must be two!!!");
    }
}

void Touch::midPoint(Point2 *midPoint, GLuint pointerCount, GLfloat x1, GLfloat y1, GLfloat x2,
                     GLfloat y2) {
    if (pointerCount == 2) {
        midPoint->x = (x2 + x1) / 2;
        midPoint->y = (y2 + y1) / 2;
    } else {
        midPoint->x = 0;
        midPoint->y = 0;
        LOGE("[touch:midPoint]Count of pointer must be two!!!");
    }
}

GLuint64 Touch::getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void* Touch::thread_fun(void *arg) {
    Touch *touch = (Touch *)arg;
    while(!touch->bExitThread) {
        touch->step -= 0.002;
        if(abs(touch->mDeltaX) > MIN) {
            touch->mDeltaX -= touch->mDeltaX > 0 ? touch->step : -touch->step;
            touch->mDeltaY = 0;
        } else if(abs(touch->mDeltaY) > MIN){
            touch->mDeltaX = 0;
            touch->mDeltaY -= touch->mDeltaY > 0? touch->step : -touch->step;
        } else {
            touch->bExitThread = GL_TRUE;
        }
        touch->mTransformBean->degreeX -= touch->mDeltaX;
        touch->mTransformBean->degreeY -= touch->mDeltaY;
        touch->sleep(20);
    }

    pthread_kill(touch->pThreadForAnim, 0);
    return NULL;
}

void Touch::sleep(GLuint ms) {
    gettimeofday(&now, NULL);
    now.tv_usec += 1000*ms;
    if (now.tv_usec > 1000000) {
        now.tv_sec += now.tv_usec / 1000000;
        now.tv_usec %= 1000000;
    }

    outtime.tv_sec = now.tv_sec;
    outtime.tv_nsec = now.tv_usec * 1000;

    pthread_cond_timedwait(&cond, &mutex, &outtime);
}

void Touch::fling(GLfloat vx, GLfloat vy) {
    float absVx = abs(vx);
    float absVy = abs(vy);
    if(absVx > absVy){
        mDeltaX = -vx * DEGREE_PER_1000PX / 1000;
        mDeltaY = 0;
    } else {
        mDeltaX = 0;
        mDeltaY = vy * DEGREE_PER_1000PX / 1000;
    }

    if(mDeltaX != 0 || mDeltaY != 0){
        step = MIN;
        anim();
    }
}

void Touch::anim() {
    if(pThreadForAnim != 0) {
        pthread_detach(pThreadForAnim);
        pThreadForAnim = 0;
    }
    bExitThread = GL_FALSE;
    pthread_create(&pThreadForAnim, NULL, thread_fun, this);
}