#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl3.h>
#include <math.h>
#include "log.h"

#define BUFFER_SIZE 1024

class File {
public:
    File(GLchar *fileName);

    ~File();
    /**
     *
     * buffer   :数据buffer
     * totalSize:所有数据的个数
     * unitSize :一个数组的数据个数
     * size     :实际统计的数据个数
     *
     */
    void getBuffer(GLfloat *buffer, GLuint *totalSize, GLuint *unitSize, GLuint *size);
    void getBuffer(double *buffer, GLuint *totalSize, GLuint *unitSize, GLuint *size);
    void getBufferSize(GLuint *totalSize, GLuint *unitSize);

private:
    FILE *pFile;
};

#endif //FILE_H
