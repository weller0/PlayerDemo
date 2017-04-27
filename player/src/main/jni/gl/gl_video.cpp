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

    pBeanOriginal->mProgramHandle = createProgram(gVideoVertexShader, gVideoFragmentShader);
    LOGI("[Video:loadShader]pBeanOriginal->mProgramHandle=%d", pBeanOriginal->mProgramHandle);
    pBeanOriginal->eTextureTarget = GL_TEXTURE_EXTERNAL_OES;
}

