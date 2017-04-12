#include "bean/float_buffer.h"

FloatBuffer::FloatBuffer() {
    bufCount = 0;
    pBuffer = (_FloatBuffer **) malloc(3 * sizeof(_FloatBuffer));
}

FloatBuffer::~FloatBuffer() {
    if(pBuffer != NULL) {
        for (GLuint i = 0; i < bufCount; i++) {
            delete pBuffer[i];
        }
        delete pBuffer;
    }
}

void FloatBuffer::setBuffer(_FloatBuffer *buffer) {
    pBuffer[bufCount] = buffer;
    bufCount++;
}

_FloatBuffer *FloatBuffer::getBuffer(GLuint index) {
    if (index >= bufCount) index = bufCount - 1;
    return pBuffer[index];
}

GLuint FloatBuffer::getSize() {
    return bufCount;
}

void FloatBuffer::updateBuffer(GLfloat *buf, GLuint totalSize,
                               GLuint unitSize, GLuint unitPointSize) {
    if(pBuffer != NULL) {
        for (GLuint i = 0; i < bufCount; i++) {
            delete pBuffer[i];
        }
    }
    bufCount = 0;
    for (GLuint i = 0; i < totalSize / unitSize; i++) {
        _FloatBuffer *buffer = new _FloatBuffer();
        buffer->buffer = buf + i * unitSize / sizeof(GLfloat);
        buffer->bufferSize = unitSize;
        buffer->pointSize = unitSize / sizeof(GLfloat) / unitPointSize;
        setBuffer(buffer);
    }
}