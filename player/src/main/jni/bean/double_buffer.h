#ifndef DOUBLE_BUFFER_H
#define DOUBLE_BUFFER_H

#include <jni.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include "log.h"

typedef struct {
    double *buffer;
    GLuint bufferSize;
    GLuint pointSize;
} _DoubleBuffer;

class DoubleBuffer {
public:
    DoubleBuffer();

    ~DoubleBuffer();

    void setBuffer(_DoubleBuffer *buffer);

    _DoubleBuffer *getBuffer(GLuint index);

    GLuint getSize();

    /**
     * buf          :数据
     * totalSize    :数据总大小
     * unitSize     :单个数组的数据总大小(如果就一个数组，其值就等于unitSize)
     * unitPointSize:一个点包含数据个数(顶点坐标3个，纹理坐标2个)
     * typeSize     :数据类型大小(GLDouble为4)
     *
     */
    void updateBuffer(double *buf, GLuint totalSize, GLuint unitSize,
                      GLuint unitPointSize, GLuint typeSize = sizeof(double));
private:
    _DoubleBuffer **pBuffer;
    GLuint bufCount;
};

#endif //DOUBLE_BUFFER_H
