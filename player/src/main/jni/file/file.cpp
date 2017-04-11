#include "file/file.h"

File::File() {
}

void File::getBufferSize(GLchar *fileName, GLuint *totalSize, GLuint *unitSize) {
    FILE *pFile = fopen(fileName, (char *) "r");
    if(pFile == NULL){
        LOGE("[File:getBufferSize]%s is not exist", fileName);
        return;
    }
    unsigned char *tmpBuffer = (unsigned char *) malloc(BUFFER_SIZE * sizeof(unsigned char));
    int typeCount = 0;
    int pointer = 0;
    bool isPointer = false;
    float tmp = 0;
    int alreadyReadSize = 0;

    while (fread(tmpBuffer, 1, BUFFER_SIZE, pFile)) {
        int readSize = ftell(pFile);
        int currSize = readSize - alreadyReadSize;
        alreadyReadSize = readSize;
        for (int i = 0; i < currSize; i++) {
            char c = *(tmpBuffer + i);
            if (isPointer) {
                pointer++;
            }
            if (c >= '0' && c <= '9') {
                if (pointer == 0) {
                    tmp = tmp * 10 + c - '0';
                } else {
                    tmp = tmp + (c - '0') / powf(10, pointer);
                }
            } else if (c == '.') {
                isPointer = true;
            } else if (c == ';' || c == ',') {
                typeCount++;
                switch (typeCount) {
                    case 1:
                        *totalSize = (int) tmp;
                        break;
                    case 2:
                        *unitSize = (int) tmp;
                        goto go_out;
                }
                isPointer = false;
                pointer = 0;
                tmp = 0;
            } else {
                isPointer = false;
                pointer = 0;
                tmp = 0;
            }
        }
    }
    go_out:
    fclose(pFile);
    free(tmpBuffer);
}

void File::getBuffer(GLchar *fileName, GLfloat *buffer, GLuint *totalSize, GLuint *unitSize,
                     GLuint *size) {
    FILE *pFile = fopen(fileName, (char *) "r");
    if(pFile == NULL){
        LOGE("[File:getBuffer]%s is not exist", fileName);
        return;
    }
    unsigned char *tmpBuffer = (unsigned char *) malloc(BUFFER_SIZE * sizeof(unsigned char));
    int dataBufCount = 0;
    int typeCount = 0;
    int pointer = 0;
    bool isPointer = false;
    bool isNegative = false;
    float tmp = 0;
    int alreadyReadSize = 0;

    while (fread(tmpBuffer, 1, BUFFER_SIZE, pFile)) {
        int readSize = ftell(pFile);
        int currSize = readSize - alreadyReadSize;
        alreadyReadSize = readSize;
        for (int i = 0; i < currSize; i++) {
            char c = *(tmpBuffer + i);
            if (isPointer) {
                pointer++;
            }
            if (c >= '0' && c <= '9') {
                if (pointer == 0) {
                    tmp = tmp * 10 + c - '0';
                } else {
                    tmp = tmp + (c - '0') / powf(10, pointer);
                }
            } else if (c == '-') {
                isNegative = true;
            } else if (c == '.') {
                isPointer = true;
            } else if (c == ';' || c == ',') {
                typeCount++;
                switch (typeCount) {
                    case 1:
                        *totalSize = (int) tmp;
                        break;
                    case 2:
                        *unitSize = (int) tmp;
                        break;
                    default:
                        buffer[dataBufCount] = isNegative ? (-tmp) : tmp;
                        dataBufCount++;
                        break;
                }
                isPointer = false;
                pointer = 0;
                tmp = 0;
                isNegative = false;
            } else {
                isPointer = false;
                pointer = 0;
                tmp = 0;
                isNegative = false;
            }
        }
    }

    fclose(pFile);
    *size = dataBufCount;
    free(tmpBuffer);
}

File::~File() {

}