#include "bean/double_buffer.h"

DoubleBuffer::DoubleBuffer() {
    bufCount = 0;
    pBuffer = (_DoubleBuffer **) malloc(3 * sizeof(_DoubleBuffer));
}

DoubleBuffer::~DoubleBuffer() {
    if(pBuffer != NULL) {
        for (GLuint i = 0; i < bufCount; i++) {
            delete pBuffer[i];
        }
        delete pBuffer;
    }
}

void DoubleBuffer::setBuffer(_DoubleBuffer *buffer) {
    pBuffer[bufCount] = buffer;
    bufCount++;
}

_DoubleBuffer *DoubleBuffer::getBuffer(GLuint index) {
    if (index >= bufCount) index = bufCount - 1;
    return pBuffer[index];
}

GLuint DoubleBuffer::getSize() {
    return bufCount;
}

void DoubleBuffer::updateBuffer(double *buf, GLuint totalSize, GLuint unitSize,
                               GLuint unitPointSize, GLuint typeSize) {
    if(pBuffer != NULL) {
        for (GLuint i = 0; i < bufCount; i++) {
            delete pBuffer[i];
        }
    }
    bufCount = 0;
    for (GLuint i = 0; i < totalSize / unitSize; i++) {
        _DoubleBuffer *buffer = new _DoubleBuffer();
        buffer->buffer = buf + i * unitSize / typeSize;
        buffer->bufferSize = unitSize;
        buffer->pointSize = unitSize / typeSize / unitPointSize;
        setBuffer(buffer);
    }
}