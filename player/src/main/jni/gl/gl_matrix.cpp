#include "gl_matrix.h"

void _multiply(GLfloat *r, const GLfloat *lhs, const GLfloat *rhs) {
    for (int i = 0; i < 4; i++) {
        const GLfloat rhs_i0 = rhs[I(i, 0)];
        GLfloat ri0 = lhs[I(0, 0)] * rhs_i0;
        GLfloat ri1 = lhs[I(0, 1)] * rhs_i0;
        GLfloat ri2 = lhs[I(0, 2)] * rhs_i0;
        GLfloat ri3 = lhs[I(0, 3)] * rhs_i0;
        for (int j = 1; j < 4; j++) {
            const GLfloat rhs_ij = rhs[I(i, j)];
            ri0 += lhs[I(j, 0)] * rhs_ij;
            ri1 += lhs[I(j, 1)] * rhs_ij;
            ri2 += lhs[I(j, 2)] * rhs_ij;
            ri3 += lhs[I(j, 3)] * rhs_ij;
        }
        r[I(i, 0)] = ri0;
        r[I(i, 1)] = ri1;
        r[I(i, 2)] = ri2;
        r[I(i, 3)] = ri3;
    }
}

GLfloat _length(GLfloat x, GLfloat y, GLfloat z) {
    return (GLfloat) sqrt(x * x + y * y + z * z);
}

void _setIdentity(GLfloat *sm, int smOffset) {
    for (int i = 0; i < 16; i++) {
        sm[smOffset + i] = 0;
    }
    for (int i = 0; i < 16; i += 5) {
        sm[smOffset + i] = 1.0f;
    }
}

void _scale(GLfloat *m, int mOffset, GLfloat x, GLfloat y, GLfloat z) {
    for (int i = 0; i < 4; i++) {
        int mi = mOffset + i;
        m[mi] *= x;
        m[4 + mi] *= y;
        m[8 + mi] *= z;
    }
}

void _translate(GLfloat *m, int mOffset, GLfloat x, GLfloat y, GLfloat z) {
    for (int i = 0; i < 4; i++) {
        int mi = mOffset + i;
        m[12 + mi] += m[mi] * x + m[4 + mi] * y + m[8 + mi] * z;
    }
}

void _rotate(GLfloat *rm, int rmOffset, GLfloat a, GLfloat x, GLfloat y, GLfloat z) {
    rm[rmOffset + 3] = 0;
    rm[rmOffset + 7] = 0;
    rm[rmOffset + 11] = 0;
    rm[rmOffset + 12] = 0;
    rm[rmOffset + 13] = 0;
    rm[rmOffset + 14] = 0;
    rm[rmOffset + 15] = 1;
    a *= (GLfloat) (M_PI / 180.0f);
    GLfloat s = (GLfloat) sin(a);
    GLfloat c = (GLfloat) cos(a);
    if (1.0f == x && 0.0f == y && 0.0f == z) {
        rm[rmOffset + 5] = c;
        rm[rmOffset + 10] = c;
        rm[rmOffset + 6] = s;
        rm[rmOffset + 9] = -s;
        rm[rmOffset + 1] = 0;
        rm[rmOffset + 2] = 0;
        rm[rmOffset + 4] = 0;
        rm[rmOffset + 8] = 0;
        rm[rmOffset + 0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        rm[rmOffset + 0] = c;
        rm[rmOffset + 10] = c;
        rm[rmOffset + 8] = s;
        rm[rmOffset + 2] = -s;
        rm[rmOffset + 1] = 0;
        rm[rmOffset + 4] = 0;
        rm[rmOffset + 6] = 0;
        rm[rmOffset + 9] = 0;
        rm[rmOffset + 5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        rm[rmOffset + 0] = c;
        rm[rmOffset + 5] = c;
        rm[rmOffset + 1] = s;
        rm[rmOffset + 4] = -s;
        rm[rmOffset + 2] = 0;
        rm[rmOffset + 6] = 0;
        rm[rmOffset + 8] = 0;
        rm[rmOffset + 9] = 0;
        rm[rmOffset + 10] = 1;
    } else {
        GLfloat len = _length(x, y, z);
        if (1.0f != len) {
            GLfloat recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        GLfloat nc = 1.0f - c;
        GLfloat xy = x * y;
        GLfloat yz = y * z;
        GLfloat zx = z * x;
        GLfloat xs = x * s;
        GLfloat ys = y * s;
        GLfloat zs = z * s;
        rm[rmOffset + 0] = x * x * nc + c;
        rm[rmOffset + 4] = xy * nc - zs;
        rm[rmOffset + 8] = zx * nc + ys;
        rm[rmOffset + 1] = xy * nc + zs;
        rm[rmOffset + 5] = y * y * nc + c;
        rm[rmOffset + 9] = yz * nc - xs;
        rm[rmOffset + 2] = zx * nc - ys;
        rm[rmOffset + 6] = yz * nc + xs;
        rm[rmOffset + 10] = z * z * nc + c;
    }
}

void _setLookAt(GLfloat *rm, int rmOffset,
                GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
                GLfloat centerX, GLfloat centerY, GLfloat centerZ,
                GLfloat upX, GLfloat upY, GLfloat upZ) {

    // See the OpenGL GLUT documentation for gluLookAt for a description
    // of the algorithm. We implement it in a straightforward way:

    GLfloat fx = centerX - eyeX;
    GLfloat fy = centerY - eyeY;
    GLfloat fz = centerZ - eyeZ;

    // Normalize f
    GLfloat rlf = 1.0f / _length(fx, fy, fz);
    fx *= rlf;
    fy *= rlf;
    fz *= rlf;

    // compute s = f x up (x means "cross product")
    GLfloat sx = fy * upZ - fz * upY;
    GLfloat sy = fz * upX - fx * upZ;
    GLfloat sz = fx * upY - fy * upX;

    // and normalize s
    GLfloat rls = 1.0f / _length(sx, sy, sz);
    sx *= rls;
    sy *= rls;
    sz *= rls;

    // compute u = s x f
    GLfloat ux = sy * fz - sz * fy;
    GLfloat uy = sz * fx - sx * fz;
    GLfloat uz = sx * fy - sy * fx;

    rm[rmOffset + 0] = sx;
    rm[rmOffset + 1] = ux;
    rm[rmOffset + 2] = -fx;
    rm[rmOffset + 3] = 0.0f;

    rm[rmOffset + 4] = sy;
    rm[rmOffset + 5] = uy;
    rm[rmOffset + 6] = -fy;
    rm[rmOffset + 7] = 0.0f;

    rm[rmOffset + 8] = sz;
    rm[rmOffset + 9] = uz;
    rm[rmOffset + 10] = -fz;
    rm[rmOffset + 11] = 0.0f;

    rm[rmOffset + 12] = 0.0f;
    rm[rmOffset + 13] = 0.0f;
    rm[rmOffset + 14] = 0.0f;
    rm[rmOffset + 15] = 1.0f;

    _translate(rm, rmOffset, -eyeX, -eyeY, -eyeZ);
}

void _perspective(GLfloat *m, int offset,
                  GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    GLfloat f = 1.0f / (GLfloat) tan(fovy * (M_PI / 360.0));
    GLfloat rangeReciprocal = 1.0f / (zNear - zFar);

    m[offset + 0] = f / aspect;
    m[offset + 1] = 0.0f;
    m[offset + 2] = 0.0f;
    m[offset + 3] = 0.0f;

    m[offset + 4] = 0.0f;
    m[offset + 5] = f;
    m[offset + 6] = 0.0f;
    m[offset + 7] = 0.0f;

    m[offset + 8] = 0.0f;
    m[offset + 9] = 0.0f;
    m[offset + 10] = (zFar + zNear) * rangeReciprocal;
    m[offset + 11] = -1.0f;

    m[offset + 12] = 0.0f;
    m[offset + 13] = 0.0f;
    m[offset + 14] = 2.0f * zFar * zNear * rangeReciprocal;
    m[offset + 15] = 0.0f;
}

Matrix::Matrix() {
    mTransformMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    mProjectionMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    mCameraMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    setIdentity(mTransformMatrix);
    setIdentity(mProjectionMatrix);
    setIdentity(mCameraMatrix);
}

Matrix::~Matrix() {
    if (mTransformMatrix != NULL) free(mTransformMatrix);
    if (mProjectionMatrix != NULL) free(mProjectionMatrix);
    if (mCameraMatrix != NULL) free(mCameraMatrix);
}

void Matrix::lookAt(GLfloat cx, GLfloat cy, GLfloat cz,
                    GLfloat tx, GLfloat ty, GLfloat tz,
                    GLfloat upx, GLfloat upy, GLfloat upz) {
    _setLookAt(mCameraMatrix, 0,
               cx, cy, cz,
               tx, ty, tz,
               upx, upy, upz);
}

void Matrix::perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
    _perspective(mProjectionMatrix, 0, fovy, aspect, near, far);
}

GLfloat *Matrix::getCameraMatrix() {
    return mCameraMatrix;
}

GLfloat* Matrix::getProjectionMatrix() {
    return mProjectionMatrix;
}

GLfloat* Matrix::getTransformMatrix() {
    return mTransformMatrix;
}

void Matrix::setIdentity(GLfloat *sm) {
    _setIdentity(sm, 0);
}

void Matrix::setIdentity() {
    setIdentity(mTransformMatrix);
}

void Matrix::rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat *tmpMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    setIdentity(tmpMatrix);
    _rotate(tmpMatrix, 0, a, x, y, z);
    _multiply(mTransformMatrix, tmpMatrix, mTransformMatrix);
    free(tmpMatrix);
}

void Matrix::scale(GLfloat x, GLfloat y, GLfloat z) {
    GLfloat *tmpMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    setIdentity(tmpMatrix);
    _scale(tmpMatrix, 0, x, y, z);
    _multiply(mTransformMatrix, tmpMatrix, mTransformMatrix);
    free(tmpMatrix);
}

void Matrix::translate(GLfloat x, GLfloat y, GLfloat z) {
    GLfloat *tmpMatrix = (GLfloat *) malloc(16 * sizeof(GLfloat));
    setIdentity(tmpMatrix);
    _translate(tmpMatrix, 0, x, y, z);
    _multiply(mTransformMatrix, tmpMatrix, mTransformMatrix);
    free(tmpMatrix);
}