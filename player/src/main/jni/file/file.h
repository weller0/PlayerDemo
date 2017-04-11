#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl3.h>
#include <math.h>
#include "log.h"

#define BUFFER_SIZE 64

class File {
public:
    File();

    ~File();

    void getBuffer(GLchar *fileName, GLfloat *buffer, GLuint *totalSize, GLuint *unitSize,
                   GLuint *size);
    void getBufferSize(GLchar *fileName, GLuint *totalSize, GLuint *unitSize);
};

#endif //FILE_H
