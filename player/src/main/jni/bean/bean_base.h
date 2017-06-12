#ifndef BEAN_BASE_H
#define BEAN_BASE_H

#include <GLES3/gl3.h>

#define TO_DEGREES(a)       (a) * 180 / M_PI
#define TO_RADIANS(a)       (a) / 180 * M_PI

static const GLuint SM_BASE = -2;
static const GLuint SM_NULL = SM_BASE + 1;
static const GLuint SM_NORMAL = SM_BASE + 2;
static const GLuint SM_ASTEROID = SM_BASE + 3;
static const GLuint SM_SPHERE = SM_BASE + 4;
static const GLuint SM_OTHER = SM_BASE + 5;
static const GLuint SM_MAX = SM_BASE + 6;

static const GLuint CS_BASE = -2;
static const GLuint CS_NULL = CS_BASE + 1;
static const GLuint CS_DRAG = CS_BASE + 2;
static const GLuint CS_DRAG_ZOOM = CS_BASE + 3;
static const GLuint CS_SENSOR = CS_BASE + 4;
static const GLuint CS_MAX = CS_BASE + 5;

static const GLuint RR_BASE = -2;
static const GLuint RR_NULL = RR_BASE + 1;
static const GLuint RR_4K = RR_BASE + 2;
static const GLuint RR_2K = RR_BASE + 3;
static const GLuint RR_1080P = RR_BASE + 4;
static const GLuint RR_720P = RR_BASE + 5;
static const GLuint RR_MAX = RR_BASE + 6;

static const GLfloat FOV_MIN = 20;
static const GLfloat FOV_MAX = 70;
static const GLfloat FOV_DEFAULT = 54;

typedef struct {
    GLboolean isLeft;
    GLboolean isUseBitmap;
    GLuint mShowMode;
    GLuint mCtrlStyle;
    GLuint mResolutionRatio;
    char *mAppPath;
} SettingsBean;

typedef struct {
    GLfloat scale;
    GLfloat degreeX;
    GLfloat degreeY;
    GLfloat degreeZ;
    GLfloat fov;
} TransformBean;

typedef struct {
    GLfloat x;
    GLfloat y;
} Point2;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Point3;

#endif //BEAN_BASE_H
