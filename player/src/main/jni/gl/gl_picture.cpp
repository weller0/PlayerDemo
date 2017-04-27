#include <android/bitmap.h>
#include "gl/gl_picture.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

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
    pBeanOriginal->mProgramHandle = createProgram(gPicVertexShader, gPicFragmentShader);
    LOGI("[Picture:loadShader]pBeanOriginal->mProgramHandle=%d", pBeanOriginal->mProgramHandle);
}

void Picture::prepareDraw(Bitmap *bmp) {
    if (bFirstFrame && bmp != NULL) {
        bFirstFrame = GL_FALSE;
        glBindTexture(pBeanOriginal->eTextureTarget, pBeanOriginal->mTextureId);
        glTexImage2D(pBeanOriginal->eTextureTarget, 0, GL_RGBA,
                     bmp->bitmapInfo.width, bmp->bitmapInfo.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, bmp->pixels);
    }
}

