#include "gl/gl_video.h"

Video::Video(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[Video] +");
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
    pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) videoTexture, sizeof(videoTexture),
                                               sizeof(videoTexture[0]), 2);
    pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) videoVertex, sizeof(videoVertex),
                                              sizeof(videoVertex[0]), 3);
    pBeanProcess->bUpdateBuffer = GL_TRUE;
}

