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

    /**
     * buf          :数据
     * totalSize    :数据总大小
     * unitSize     :单个数组的数据总大小(如果就一个数组，其值就等于unitSize)
     * unitPointSize:一个点包含数据个数(顶点坐标3个，纹理坐标2个)
     * typeSize     :数据类型大小(GLfloat为4)
     *
     */
    void updateBuffer(GLfloat *buf, GLuint totalSize, GLuint unitSize,
                      GLuint unitPointSize, GLuint typeSize = sizeof(GLfloat));
private:
    _FloatBuffer **pBuffer;
    GLuint bufCount;
};

#endif //FLOAT_BUFFER_H
