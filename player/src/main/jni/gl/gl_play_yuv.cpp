#include <ffmpeg/libavcodec/avcodec.h>
#include "gl/gl_play_yuv.h"

PlayYuv::PlayYuv(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[PlayYuv] +");
    bFirstFrame = GL_TRUE;
    bFirstFrameForCompose = GL_TRUE;
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

PlayYuv::~PlayYuv() {
    LOGI("[PlayYuv] -");

    pthread_mutex_destroy(&mutex);
}

void PlayYuv::loadShader() {
    GLRenderer::loadShader();
    pBeanProcess->mProgramHandle = createProgram(gYuvAVertexShader, gYuvAFragmentShader);
    // 获取投影、Camera、变换句柄
    pBeanProcess->mProjectionHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                           "projection");
    pBeanProcess->mCameraHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                       "camera");
    pBeanProcess->mTransformHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                          "transform");
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                      "light");
    LOGI("[PlayYuv:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

GLuint PlayYuv::updateTextureAuto() {
    return 15;
}

GLboolean PlayYuv::prepareDraw(Bitmap *bmp) {
    pYuvFrame = funcGetFrame();
    if (pYuvFrame != NULL && pYuvFrame->width > 0) {
        Mat yuvImg = Mat(pYuvFrame->height / 2, pYuvFrame->width / 2, CV_8UC3, pYuvFrame->data[1]);
        prepareComposeTexture(yuvImg);

        if (bFirstFrame) {
            bFirstFrame = GL_FALSE;
            glBindTexture(GL_TEXTURE_2D, pBeanProcess->mTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         pYuvFrame->width, pYuvFrame->height, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, pYuvFrame->data[0]);
        } else {
            glBindTexture(GL_TEXTURE_2D, pBeanProcess->mTextureId);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            pYuvFrame->width, pYuvFrame->height,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
                            pYuvFrame->data[0]);
        }
        return GL_TRUE;
    } else {
        return GL_FALSE;
    }
}

void PlayYuv::drawForYUV(GLBean *glBean) {
    glUseProgram(glBean->mProgramHandle);
    for (GLuint i = 0; i < glBean->pVertexBuffer->getSize(); i++) {
        // 绑定VAO，绑定之后开始绘制
        glBindVertexArray(glBean->pVAO[i]);
        checkGLError("draw glBindVertexArray +");

        // 投影、Camera、变换赋值
        if (glBean->pTransformBean != NULL) {
            glBean->pMatrix->perspective(glBean->pTransformBean->fov,
                                         (GLfloat) mWindowWidth / (GLfloat) mWindowHeight,
                                         0.1,
                                         100);
            glBean->pMatrix->setIdentity();
            glBean->pMatrix->rotate(glBean->pTransformBean->degreeY, 1, 0, 0);
            glBean->pMatrix->rotate(glBean->pTransformBean->degreeX, 0, 1, 0);
        }
        if (glBean->mProjectionHandle != -1) {
            glUniformMatrix4fv(glBean->mProjectionHandle, 1, GL_FALSE,
                               glBean->pMatrix->getProjectionMatrix());
        }
        if (glBean->mCameraHandle != -1) {
            glUniformMatrix4fv(glBean->mCameraHandle, 1, GL_FALSE,
                               glBean->pMatrix->getCameraMatrix());
        }
        if (glBean->mTransformHandle != -1) {
            glUniformMatrix4fv(glBean->mTransformHandle, 1, GL_FALSE,
                               glBean->pMatrix->getTransformMatrix());
        }
        if (glBean->mLightHandle != -1) {
            glUniform3f(glBean->mLightHandle, 1, 1, 1);
        }
        if (glBean->mComposeTextureId != 0 && i > 0) {
            glBindTexture(glBean->eTextureTarget, glBean->mComposeTextureId);
        } else {
            glBindTexture(GL_TEXTURE_2D, glBean->mTextureId);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, glBean->pVertexBuffer->getBuffer(i)->pointSize);
        // 解绑VAO
        glBindVertexArray(0);
        checkGLError("draw glBindVertexArray -");
    }
}

GLboolean PlayYuv::useYUVDraw() {
    return GL_TRUE;
}

void PlayYuv::prepareComposeTexture(Mat original) {
    if (bFirstFrameForCompose) {
        bFirstFrameForCompose = GL_FALSE;
        initCompose(original.rows, original.cols);
        Mat out = compose(original);
        glBindTexture(GL_TEXTURE_2D, pBeanProcess->mComposeTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     out.cols, out.rows, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, out.data);
    } else {
        Mat out = compose(original);
        glBindTexture(GL_TEXTURE_2D, pBeanProcess->mComposeTextureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        out.cols, out.rows,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, out.data);
    }
}

void PlayYuv::initCompose(GLuint w, GLuint h) {

}

Mat PlayYuv::compose(Mat original) {
    return original;
}