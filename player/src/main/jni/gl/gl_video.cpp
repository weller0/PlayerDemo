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
    File *file = new File((char *) "/storage/emulated/0/Movies/texcoord_buffer_1");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *textureBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(textureBuffer, &totalSize, &unitSize, &bufSize);
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
    file = new File((char *) "/storage/emulated/0/Movies/vertex_buffer_1");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *vertexBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(vertexBuffer, &totalSize, &unitSize, &bufSize);
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

