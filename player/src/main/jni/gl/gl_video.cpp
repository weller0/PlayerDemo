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
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle, "light");
    pBeanProcess->eTextureTarget = GL_TEXTURE_EXTERNAL_OES;
    LOGI("[Picture:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

