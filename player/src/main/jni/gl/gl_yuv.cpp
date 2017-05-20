#include "gl/gl_yuv.h"

PictureYuv::PictureYuv(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[PictureYuv] +");
    bFirstFrame = GL_TRUE;
    mSettingsBean = settingsBean;

    pthread_mutex_init(&mutex, NULL);
    char so32Path[256] = {0};
    sprintf(so32Path, "%s/lib/arm/libijkplayer.so", settingsBean->mAppPath);
    pSO = dlopen(so32Path, RTLD_NOW);
    if (pSO == NULL) {
        LOGE("[PictureYuv]%s", dlerror());
        char so64Path[256] = {0};
        sprintf(so64Path, "%s/lib/arm64/libijkplayer.so", settingsBean->mAppPath);
        pSO = dlopen(so64Path, RTLD_NOW);
        if (pSO == NULL) {
            LOGE("[PictureYuv]%s", dlerror());
        }
    }
    if(pSO != NULL) {
        funcGetFrame = (AVFrame *(*)()) dlsym(pSO, "ijkmp_get_frame");
    }
}

PictureYuv::~PictureYuv() {
    LOGI("[PictureYuv] -");

    pthread_mutex_destroy(&mutex);
    if(pSO) {
        dlclose(pSO);
    }
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
//    mTexHandleY = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_y");
//    mTexHandleU = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_u");
//    mTexHandleV = glGetUniformLocation(pBeanProcess->mProgramHandle, "tex_v");
    LOGI("[PictureYuv:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

GLboolean PictureYuv::prepareDraw(Bitmap *bmp) {
    if(pSO == NULL) return GL_FALSE;
    pFrame = funcGetFrame();
    if (pFrame != NULL) {
        LOGI("[PictureYuv:prepareDraw]frame (%d, %d), format=%d",
             pFrame->width, pFrame->height, pFrame->format);
        //Mat yuvImg = Mat(pFrame->height*3/2, pFrame->width, CV_8UC3, pFrame->data);
        //Mat bgrImg;
        //cvtColor(yuvImg, bgrImg, CV_YUV2BGR_NV21);
        if (bFirstFrame) {
            bFirstFrame = GL_FALSE;
//            createTextureForYUV();
//            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pBeanProcess->mTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         pFrame->width, pFrame->height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE,
                         pFrame->data);

//            glActiveTexture(GL_TEXTURE1);
//            glBindTexture(GL_TEXTURE_2D, mTextureU);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
//                         pFrame->width / 2, pFrame->height / 2, 0,
//                         GL_LUMINANCE, GL_UNSIGNED_BYTE,
//                         pFrame->data + pFrame->width * pFrame->height);

//            glActiveTexture(GL_TEXTURE2);
//            glBindTexture(GL_TEXTURE_2D, mTextureV);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
//                         pFrame->width / 2, pFrame->height / 2, 0,
//                         GL_LUMINANCE, GL_UNSIGNED_BYTE,
//                         pFrame->data + pFrame->width * pFrame->height * 5 / 4);

        } else {
//            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pBeanProcess->mTextureId);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            pFrame->width, pFrame->height,
                            GL_RGB, GL_UNSIGNED_BYTE,
                            pFrame->data);

//            glActiveTexture(GL_TEXTURE1);
//            glBindTexture(GL_TEXTURE_2D, mTextureU);
//            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
//                            pFrame->width / 2, pFrame->height / 2,
//                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
//                            pFrame->data + pFrame->width * pFrame->height);
//
//            glActiveTexture(GL_TEXTURE2);
//            glBindTexture(GL_TEXTURE_2D, mTextureV);
//            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
//                            pFrame->width / 2, pFrame->height / 2,
//                            GL_LUMINANCE, GL_UNSIGNED_BYTE,
//                            pFrame->data + pFrame->width * pFrame->height * 5 / 4);
        }
        return GL_TRUE;
    } else {
        return GL_FALSE;
    }
}

GLuint PictureYuv::updateTextureAuto() {
    return 10;
}

void PictureYuv::drawForYUV(GLBean *glBean) {
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
        //if (glBean->mComposeTextureId != 0 && i > 0) {
        //    glBindTexture(glBean->eTextureTarget, glBean->mComposeTextureId);
       // } else {
            glBindTexture(glBean->eTextureTarget, glBean->mTextureId);
            // bind textures
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, mTextureY);
//            glUniform1i(mTexHandleY, 0);
//
//            glActiveTexture(GL_TEXTURE1);
//            glBindTexture(GL_TEXTURE_2D, mTextureU);
//            glUniform1i(mTexHandleU, 1);
//
//            glActiveTexture(GL_TEXTURE2);
//            glBindTexture(GL_TEXTURE_2D, mTextureV);
//            glUniform1i(mTexHandleV, 2);
       // }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, glBean->pVertexBuffer->getBuffer(i)->pointSize);
        // 解绑VAO
        glBindVertexArray(0);
        checkGLError("draw glBindVertexArray -");
    }
}

GLboolean PictureYuv::useYUVDraw() {
    return GL_TRUE;
}

void PictureYuv::createTextureForYUV() {
    glGenTextures(1, &mTextureY);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &mTextureU);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTextureU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &mTextureV);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mTextureV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}