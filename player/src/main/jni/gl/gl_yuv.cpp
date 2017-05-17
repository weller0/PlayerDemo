#include "gl/gl_yuv.h"
#include "gl_renderer.h"

PictureYuv::PictureYuv(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[PictureYuv] +");
    bFirstFrame = GL_TRUE;
    mSettingsBean = settingsBean;

    pthread_mutex_init(&mutex, NULL);
    pSO = dlopen("/data/data/com.wq.playerdemo/lib/libijkplayer.so", RTLD_NOW);
    if (pSO == NULL) {
        LOGE("[jin_api]load libijkplayer.so from lib  fail!");
        pSO = dlopen("/data/data/com.wq.playerdemo/lib64/libijkplayer.so", RTLD_NOW);
        if (pSO == NULL) {
            LOGE("[jin_api]load libijkplayer.so from lib64  fail!");
            exit(0);
        }
    }
    funcGetFrame = (AVFrame *(*)()) dlsym(pSO, "ijkmp_get_frame");
}

PictureYuv::~PictureYuv() {
    LOGI("[PictureYuv] -");

    pthread_mutex_destroy(&mutex);
}

void PictureYuv::loadShader() {
    GLRenderer::loadShader();
    pBeanProcess->mProgramHandle = createProgram(gYuvVertexShader, gYuvFragmentShader);
    // 获取投影、Camera、变换句柄
    pBeanProcess->mProjectionHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                           "projection");
    pBeanProcess->mCameraHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                       "camera");
    pBeanProcess->mTransformHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                          "transform");
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                      "light");
    LOGI("[PictureYuv:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

GLboolean PictureYuv::prepareDraw(Bitmap *bmp) {
    pFrame = funcGetFrame();
    if (pFrame != NULL) {
        LOGD("[PictureYuv:prepareDraw]frame (%d, %d)", pFrame->width, pFrame->height);
        if (bFirstFrame) {
            bFirstFrame = GL_FALSE;
            createTextureForYUV();
            glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
            glTexImage2D(pBeanProcess->eTextureTarget, 0, GL_RGBA,
                         pFrame->width, pFrame->height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, pFrame->data);
        } else {
            glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
            glTexSubImage2D(pBeanProcess->eTextureTarget, 0, 0, 0,
                            pFrame->width, pFrame->height,
                            GL_RGBA, GL_UNSIGNED_BYTE, pFrame->data);
        }
        return GL_TRUE;
    } else {
        return GL_FALSE;
    }
}

GLuint PictureYuv::updateTextureAuto() {
    return 10;
}

void PictureYuv::createTextureForYUV() {
    glGenTextures(1, &mTextureY);
    glBindTexture(GL_TEXTURE_2D, mTextureY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &mTextureU);
    glBindTexture(GL_TEXTURE_2D, mTextureU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &mTextureV);
    glBindTexture(GL_TEXTURE_2D, mTextureV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}