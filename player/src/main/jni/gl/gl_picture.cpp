#include "gl/gl_picture.h"

Picture::Picture(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGD("[Picture] +");
    bFirstFrame = GL_TRUE;
}

Picture::~Picture() {
    LOGD("[Picture] -");
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
    LOGD("[Picture:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);

}

void Picture::prepareProcessBuffer() {
    LOGD("[GLRenderer:prepareProcessBuffer]");
    pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) picTexture, sizeof(picTexture),
                                               sizeof(picTexture[0]), 2);
    pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) picVertex, sizeof(picVertex),
                                              sizeof(picVertex[0]), 3);
    pBeanProcess->bUpdateBuffer = GL_TRUE;
}

void Picture::prepareDraw(Bitmap *bmp) {
    if (bFirstFrame && bmp != NULL) {
        bFirstFrame = GL_FALSE;
        glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
        glTexImage2D(pBeanProcess->eTextureTarget, 0, GL_RGBA,
                     bmp->bitmapInfo.width, bmp->bitmapInfo.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, bmp->pixels);
    }
}

