#include "file/file.h"

File::File(GLchar *fileName) {
    pFile = fopen(fileName, (char *) "r");
    if (pFile == NULL) {
        LOGE("[File]%s is not exist", fileName);
    }
    LOGD("[File]Loading file %s", fileName);
}

File::~File() {
    fclose(pFile);
}

void File::getBufferSize(GLuint *totalSize, GLuint *unitSize) {
    if (pFile == NULL) {
        LOGE("[File:getBufferSize]file is not exist");
        return;
    }

    GLchar numTmp[64] = {0};
    GLuint numSize = 0;
    GLuint alreadyReadSize = 0;
    GLuint numCount = 0;
    GLchar *tmpBuffer = (GLchar *) malloc(BUFFER_SIZE * sizeof(GLchar));
    rewind(pFile);
    while (fread(tmpBuffer, 1, BUFFER_SIZE, pFile)) {
        GLuint readSize = ftell(pFile);
        GLuint currSize = readSize - alreadyReadSize;
        alreadyReadSize = readSize;
//        LOGD("tmpBuffer : %s", tmpBuffer);
        for (GLuint i = 0; i < currSize; i++) {
            char c = *(tmpBuffer + i);
            if ((c >= '0' && c <= '9') ||
                c == '.' || c == '+' || c == '-' ||
                c == 'e' || c == 'E') {
                numTmp[numSize] = c;
                numSize++;
                numTmp[numSize] = '\0';
            } else if (numSize != 0) {
                double out = strtod(numTmp, NULL);
                if (numCount == 0) {
                    *totalSize = (GLuint) out;
                } else if (numCount == 1) {
                    *unitSize = (GLuint) out;
                } else {
                    return;
                }
                numCount++;
                numSize = 0;
                numTmp[numSize] = '\0';
            }
        }
    }
    free(tmpBuffer);
}

void File::getBuffer(GLfloat *buffer, GLuint *totalSize, GLuint *unitSize, GLuint *size) {
    if (pFile == NULL) {
        LOGE("[File:getBufferSize]file is not exist");
        return;
    }

    GLchar numTmp[64] = {0};
    GLuint numSize = 0;
    GLuint alreadyReadSize = 0;
    GLuint numCount = 0;
    GLchar *tmpBuffer = (GLchar *) malloc(BUFFER_SIZE * sizeof(GLchar));
    rewind(pFile);
    while (fread(tmpBuffer, 1, BUFFER_SIZE, pFile)) {
        GLuint readSize = ftell(pFile);
        GLuint currSize = readSize - alreadyReadSize;
        alreadyReadSize = readSize;
//        LOGD("tmpBuffer : %s", tmpBuffer);
        for (GLuint i = 0; i < currSize; i++) {
            char c = *(tmpBuffer + i);
            if ((c >= '0' && c <= '9') ||
                c == '.' || c == '+' || c == '-' ||
                c == 'e' || c == 'E') {
                numTmp[numSize] = c;
                numSize++;
                numTmp[numSize] = '\0';
            } else if (numSize != 0) {
                double out = strtod(numTmp, NULL);
                if (numCount == 0) {
                    *totalSize = (GLuint) out;
                } else if (numCount == 1) {
                    *unitSize = (GLuint) out;
                } else {
                    buffer[numCount - 2] = (GLfloat)out;
                }
                numCount++;
                numSize = 0;
                numTmp[numSize] = '\0';
            }
        }
    }
    *size = numCount - 2;
    free(tmpBuffer);
}

void File::getBuffer(double *buffer, GLuint *totalSize, GLuint *unitSize, GLuint *size) {
    if (pFile == NULL) {
        LOGE("[File:getBufferSize]file is not exist");
        return;
    }

    GLchar numTmp[64] = {0};
    GLuint numSize = 0;
    GLuint alreadyReadSize = 0;
    GLuint numCount = 0;
    GLchar *tmpBuffer = (GLchar *) malloc(BUFFER_SIZE * sizeof(GLchar));
    rewind(pFile);
    while (fread(tmpBuffer, 1, BUFFER_SIZE, pFile)) {
        GLuint readSize = ftell(pFile);
        GLuint currSize = readSize - alreadyReadSize;
        alreadyReadSize = readSize;
//        LOGD("tmpBuffer : %s", tmpBuffer);
        for (GLuint i = 0; i < currSize; i++) {
            char c = *(tmpBuffer + i);
            if ((c >= '0' && c <= '9') ||
                c == '.' || c == '+' || c == '-' ||
                c == 'e' || c == 'E') {
                numTmp[numSize] = c;
                numSize++;
                numTmp[numSize] = '\0';
            } else if (numSize != 0) {
                double out = strtod(numTmp, NULL);
                if (numCount == 0) {
                    *totalSize = (GLuint) out;
                } else if (numCount == 1) {
                    *unitSize = (GLuint) out;
                } else {
                    buffer[numCount - 2] = out;
                }
                numCount++;
                numSize = 0;
                numTmp[numSize] = '\0';
            }
        }
    }
    *size = numCount - 2;
    free(tmpBuffer);
}