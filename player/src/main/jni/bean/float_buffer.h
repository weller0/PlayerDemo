#ifndef FLOAT_BUFFER_H
#define FLOAT_BUFFER_H

#include <jni.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include "log.h"

typedef struct {
    GLfloat *buffer;
    GLuint bufferSize;
    GLuint pointSize;
} _FloatBuffer;

class FloatBuffer {
public:
    FloatBuffer();

    ~FloatBuffer();

    void setBuffer(_FloatBuffer *buffer);

    _FloatBuffer *getBuffer(GLuint index);

    GLuint getSize();

    void updateBuffer(GLfloat *buf, GLuint totalSize, GLuint unitSize, GLuint unitPointSize);
private:
    _FloatBuffer **pBuffer;
    GLuint bufCount;
};

#endif //FLOAT_BUFFER_H
