#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#include <GLES3/gl3.h>
#include <math.h>
#include <stdlib.h>
#include "log.h"

#define I(_i, _j) ((_j)+ 4*(_i))

class Matrix {
public:
    Matrix();

    ~Matrix();

    void lookAt(GLfloat cx, GLfloat cy, GLfloat cz,
                GLfloat tx, GLfloat ty, GLfloat tz,
                GLfloat upx, GLfloat upy, GLfloat upz);

    void perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);

    GLfloat *getTransformMatrix();

    GLfloat *getCameraMatrix();

    GLfloat *getProjectionMatrix();

    void setIdentity(GLfloat *sm);

    void setIdentity();

    void rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z);

    void scale(GLfloat x, GLfloat y, GLfloat z);

    void translate(GLfloat x, GLfloat y, GLfloat z);

private:
    GLfloat *mProjectionMatrix;   // 投影矩阵
    GLfloat *mCameraMatrix;       // 摄像机矩阵
    GLfloat *mTransformMatrix;    // 变形矩阵
};

#endif //GL_MATRIX_H
