#include "gl/gl_video.h"

Video::Video(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[Video] +");
    mSettingsBean = settingsBean;
}

Video::~Video() {
    LOGI("[Video] -");
}

void Video::loadShader() {
    GLRenderer::loadShader();
    pBeanProcess->mProgramHandle = createProgram(gVideoVertexShader, gVideoFragmentShader);
    // 获取投影、Camera、变换句柄
    pBeanProcess->mProjectionHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                           "projection");
    pBeanProcess->mCameraHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                       "camera");
    pBeanProcess->mTransformHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                          "transform");
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                      "light");
    pBeanProcess->eTextureTarget = GL_TEXTURE_EXTERNAL_OES;
    LOGI("[Video:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

void Video::prepareProcessBuffer() {
    LOGI("[GLRenderer:prepareProcessBuffer]");
    GLuint totalSize = 0;
    GLuint unitSize = 0;
    GLuint bufSize = 0;
    char *fileName = NULL;
    switch (mSettingsBean->mShowMode) {
        case SM_SPHERE_FRONT:
            fileName = (char *) "/storage/emulated/0/Movies/texcoord_buffer_2";
            break;
        case SM_SPHERE_FRONT_BACK:
            fileName = (char *) "/storage/emulated/0/Movies/texcoord_buffer_3";
            break;
        case SM_ORIGINAL:
            fileName = (char *) "/storage/emulated/0/Movies/texcoord_buffer_3";
            break;
        default:
            fileName = (char *) "/storage/emulated/0/Movies/texcoord_buffer_1";
            break;
    }
    File *file = new File();
    file->getBufferSize(fileName, &totalSize, &unitSize);
    GLfloat *textureBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(fileName, textureBuffer, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pTextureBuffer->updateBuffer(textureBuffer, totalSize * sizeof(GLfloat),
                                                   unitSize * sizeof(GLfloat), 2);
    } else {
        pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) videoTexture, sizeof(videoTexture),
                                                   sizeof(videoTexture[0]), 2);
    }
    delete file;

    totalSize = 0;
    unitSize = 0;
    bufSize = 0;
    switch (mSettingsBean->mShowMode) {
        case SM_SPHERE_FRONT:
            fileName = (char *) "/storage/emulated/0/Movies/vertex_buffer_2";
            break;
        case SM_SPHERE_FRONT_BACK:
            fileName = (char *) "/storage/emulated/0/Movies/vertex_buffer_3";
            break;
        case SM_ORIGINAL:
            fileName = (char *) "/storage/emulated/0/Movies/vertex_buffer_2";
            break;
        default:
            fileName = (char *) "/storage/emulated/0/Movies/vertex_buffer_1";
            break;
    }
    file = new File();
    file->getBufferSize(fileName, &totalSize, &unitSize);
    GLfloat *vertexBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(fileName, vertexBuffer, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pVertexBuffer->updateBuffer(vertexBuffer, totalSize * sizeof(GLfloat),
                                                  unitSize * sizeof(GLfloat), 3);
    } else {
        pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) videoVertex, sizeof(videoVertex),
                                                  sizeof(videoVertex[0]), 3);
    }
    delete file;

    pBeanProcess->bUpdateBuffer = GL_TRUE;
}

