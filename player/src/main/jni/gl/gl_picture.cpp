#include "gl/gl_picture.h"

Picture::Picture(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[Picture] +");
    bFirstFrame = GL_TRUE;
    mSettingsBean = settingsBean;
}

Picture::~Picture() {
    LOGI("[Picture] -");
}

void Picture::loadShader() {
    GLRenderer::loadShader();
    pBeanProcess->mProgramHandle = createProgram(gPicVertexShader, gPicFragmentShader);
    // 获取投影、Camera、变换句柄
    pBeanProcess->mProjectionHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                           "projection");
    pBeanProcess->mCameraHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                       "camera");
    pBeanProcess->mTransformHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                          "transform");
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                      "light");
    LOGI("[Picture:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

GLboolean Picture::prepareDraw(Bitmap *bmp, GLboolean updateFrameData) {
    if(!updateFrameData) return GL_TRUE;
    if (bmp != NULL) {
        if (bFirstFrame) {
            bFirstFrame = GL_FALSE;
            glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
            glTexImage2D(pBeanProcess->eTextureTarget, 0, GL_RGBA,
                         bmp->bitmapInfo.width, bmp->bitmapInfo.height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, bmp->pixels);
        }
        return GL_TRUE;
    } else {
        return GL_FALSE;
    }
}

