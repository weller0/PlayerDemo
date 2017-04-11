#include "touch.h"

Touch::Touch(SettingsBean *settingsBean) {
    mSettingsBean = settingsBean;
    mStartPoint = new Point();
    mMidPoint = new Point();
    mMode = MODE_NORMAL;
}

Touch::~Touch() {
    delete mStartPoint;
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
                mLastTime = getCurrentTime();
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
        case ACTION_POINTER_UP:
            mMode = MODE_NORMAL;
            break;
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
    return true;
}

void Touch::setDrag(TransformBean *bean, GLfloat x, GLfloat y, GLuint time) {
    // 计算拖拽的距离
    float dx = x - mStartPoint->x;
    float dy = y - mStartPoint->y;
    mStartPoint->x = x;
    mStartPoint->y = y;
    dx = TO_DEGREES(dx * time / 13000.0) * sinf(TO_RADIANS(bean->fov));
    dy = TO_DEGREES(dy * time / 13000.0) * sinf(TO_RADIANS(bean->fov));
    bean->degreeX -= dx;
    bean->degreeY -= dy;
}

void Touch::setZoom(TransformBean *bean, GLuint pointerCount,
                    GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    GLfloat dis = 0;
    distance(&dis, pointerCount, x1, y1, x2, y2);
    if (dis == mDistance) return;
    // 获取将要缩放到比例,一般是1左右
    GLfloat deltaFov = (dis - mDistance > 0 ? 1 : -1) * dis / mDistance;
    deltaFov = deltaFov * sinf(TO_RADIANS(bean->fov));
    mDistance = dis;
    bean->fov -= deltaFov;
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

void Touch::midPoint(Point *midPoint, GLuint pointerCount, GLfloat x1, GLfloat y1, GLfloat x2,
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