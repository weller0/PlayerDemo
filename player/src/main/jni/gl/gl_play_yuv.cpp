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
    mTexHandleY = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_y");
    mTexHandleU = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_u");
    mTexHandleV = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_v");
    LOGI("[PlayYuv:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

GLuint PlayYuv::updateTextureAuto() {
    return 15;
}

GLboolean PlayYuv::prepareDraw(Bitmap *bmp) {
    pYuvFrame = funcGetFrame();
    if (pYuvFrame != NULL && pYuvFrame->width > 0) {
        Mat y = Mat(pYuvFrame->width, pYuvFrame->height, CV_8UC3, pYuvFrame->data[0]);
        Mat u = Mat(pYuvFrame->width / 2, pYuvFrame->height / 2, CV_8UC3, pYuvFrame->data[1]);
        Mat v = Mat(pYuvFrame->width / 2, pYuvFrame->height / 2, CV_8UC3, pYuvFrame->data[2]);
        prepareComposeTexture(pYuvFrame);

        if (bFirstFrame) {
            bFirstFrame = GL_FALSE;
            glGenTextures(1, &mTextureY);
            glBindTexture(GL_TEXTURE_2D, mTextureY);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         pYuvFrame->width, pYuvFrame->height, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, pYuvFrame->data[0]);

            glGenTextures(1, &mTextureU);
            glBindTexture(GL_TEXTURE_2D, mTextureU);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         pYuvFrame->width / 2, pYuvFrame->height / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, pYuvFrame->data[1]);

            glGenTextures(1, &mTextureV);
            glBindTexture(GL_TEXTURE_2D, mTextureV);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         pYuvFrame->width / 2, pYuvFrame->height / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, pYuvFrame->data[2]);
        } else {
            glBindTexture(GL_TEXTURE_2D, mTextureY);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            pYuvFrame->width, pYuvFrame->height,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
                            pYuvFrame->data[0]);

            glBindTexture(GL_TEXTURE_2D, mTextureU);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            pYuvFrame->width / 2, pYuvFrame->height / 2,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
                            pYuvFrame->data[1]);

            glBindTexture(GL_TEXTURE_2D, mTextureV);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            pYuvFrame->width / 2, pYuvFrame->height / 2,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
                            pYuvFrame->data[2]);
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
        if (i > 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mComposeTextureY);
            glUniform1i(mTexHandleY, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mComposeTextureU);
            glUniform1i(mTexHandleU, 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mComposeTextureV);
            glUniform1i(mTexHandleV, 2);
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mTextureY);
            glUniform1i(mTexHandleY, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mTextureU);
            glUniform1i(mTexHandleU, 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mTextureV);
            glUniform1i(mTexHandleV, 2);
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

void PlayYuv::prepareComposeTexture(AVFrame * frame) {
    if (bFirstFrameForCompose) {
        bFirstFrameForCompose = GL_FALSE;
        initCompose(frame->width, frame->height);
//        Mat out = compose(y);

        glGenTextures(1, &mComposeTextureY);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     frame->width, frame->height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[0]);

        glGenTextures(1, &mComposeTextureU);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureU);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     frame->width / 2, frame->height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[1]);

        glGenTextures(1, &mComposeTextureV);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     frame->width / 2, frame->height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[2]);
    } else {
//        Mat out = compose(NULL);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureY);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        frame->width, frame->height,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[0]);

        glBindTexture(GL_TEXTURE_2D, mComposeTextureU);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        frame->width / 2, frame->height / 2,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[1]);

        glBindTexture(GL_TEXTURE_2D, mComposeTextureV);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        frame->width / 2, frame->height / 2,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[2]);
    }
}

void PlayYuv::initCompose(GLuint w, GLuint h) {

}

Mat PlayYuv::compose(Mat original) {
    return original;
}