#include <bean/bean_base.h>
#include "gl/gl_play_yuv.h"
#include "gl_renderer.h"

#define GL_PLAY_DEBUG 0

PlayYuv::PlayYuv(TransformBean *transformBean, SettingsBean *settingsBean)
        : GLRenderer(transformBean, settingsBean) {
    LOGI("[PlayYuv] +");
    bFirstFrame = GL_TRUE;
    bFirstFrameForCompose = GL_TRUE;
    mSettingsBean = settingsBean;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
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
    if (pSO != NULL) {
        funcGetFrame = (AVFrame *(*)()) dlsym(pSO, "ijkmp_get_frame");
    }
    bExitThread = GL_FALSE;
}

PlayYuv::~PlayYuv() {
    LOGI("[PlayYuv] -");
    bExitThread = GL_TRUE;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_detach(pThreadForTexture);
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

void *PlayYuv::thread_fun(void *arg) {
    PlayYuv *play = (PlayYuv *) arg;
    while (!play->bExitThread) {
        pthread_mutex_lock(&play->mutex);
        pthread_cond_wait(&play->cond, &play->mutex);
        pthread_mutex_unlock(&play->mutex);
        if (play->pYuvFrame != NULL) {
            play->compose(play->pYuvFrame);
        }
    }
    return NULL;
}

void PlayYuv::prepareTexture() {
    pYuvFrame = funcGetFrame();
    if (pYuvFrame != NULL && pYuvFrame->width > 0) {
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
    }
}

GLboolean PlayYuv::prepareDraw(Bitmap *bmp, GLboolean updateFrameData) {
    if (!updateFrameData) return GL_TRUE;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    prepareTexture();
    return GL_TRUE;
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
            glBean->pMatrix->lookAt(0, 0, glBean->pTransformBean->lookAtCenterZ,
                                    0, 0, -1, 0, 1, 0);
            glBean->pMatrix->setIdentity();
            glBean->pMatrix->rotate(glBean->pTransformBean->degreeX, 0, 1, 0);
            glBean->pMatrix->rotate(glBean->pTransformBean->degreeY, 1, 0, 0);
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
            glUniform3f(glBean->mLightHandle, light1y, light2y, 1);
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

void PlayYuv::prepareComposeTexture(AVFrame *frame) {
    if (bFirstFrameForCompose) {
        bFirstFrameForCompose = GL_FALSE;
        initCompose(frame->width, frame->height);
        compose(frame);

        pthread_create(&pThreadForTexture, NULL, thread_fun, this);

        glGenTextures(1, &mComposeTextureY);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     out_y.cols, out_y.rows, 0,//frame->width, frame->height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, out_y.ptr(0));

        glGenTextures(1, &mComposeTextureU);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureU);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     out_u.cols, out_u.rows, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, out_u.ptr(0));

        glGenTextures(1, &mComposeTextureV);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     out_v.cols, out_v.rows, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, out_v.ptr(0));
    } else {
//        compose(frame);
        glBindTexture(GL_TEXTURE_2D, mComposeTextureY);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        out_y.cols, out_y.rows,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, out_y.ptr(0));

        glBindTexture(GL_TEXTURE_2D, mComposeTextureU);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        out_u.cols, out_u.rows,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, out_u.ptr(0));

        glBindTexture(GL_TEXTURE_2D, mComposeTextureV);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        out_v.cols, out_v.rows,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, out_v.ptr(0));
    }
}

void PlayYuv::initCompose(GLint w, GLint h) {
    // init
    //需要参数，从文件读取
    GLuint totalSize = 0;
    GLuint unitSize = 0;
    GLuint bufSize = 0;
    File *file = new File((char *) "/storage/emulated/0/Movies/parameter");
    file->getBufferSize(&totalSize, &unitSize);
    LOGD("[File]parameter size %d, %d", totalSize, unitSize);
    double *parameter = (double *) malloc(totalSize * sizeof(double));
    file->getBuffer(parameter, &totalSize, &unitSize, &bufSize);


    //读取保存的参数
    Point2f center_all_1, center_all_2;        //两个镜头中心参数
    float rad_all_1, rad_all_2;                //镜头有效区域半径参数
    Mat RotationMatrix(3, 3, CV_64FC1);        //3D 点标定外参 旋转
    Vec3d TMatrix;                             //3D 点标定外参 平移
    double pdK;                                //3D 点标定外参 缩放

    center_all_1 = Point2f((float) parameter[0], (float) parameter[1]);
    rad_all_1 = (float) parameter[2] + 0.5;
    center_all_2 = Point2f((float) parameter[3], (float) parameter[4]);
    rad_all_2 = (float) parameter[5] - 0.58337;
    RotationMatrix.at<double>(0, 0) = parameter[6];
    RotationMatrix.at<double>(0, 1) = parameter[7];
    RotationMatrix.at<double>(0, 2) = parameter[8];
    RotationMatrix.at<double>(1, 0) = parameter[9];
    RotationMatrix.at<double>(1, 1) = parameter[10];
    RotationMatrix.at<double>(1, 2) = parameter[11];
    RotationMatrix.at<double>(2, 0) = parameter[12];
    RotationMatrix.at<double>(2, 1) = parameter[13];
    RotationMatrix.at<double>(2, 2) = parameter[14];
    TMatrix[0] = parameter[15];
    TMatrix[1] = parameter[16];
    TMatrix[2] = parameter[17];
    pdK = parameter[18];
#if GL_PLAY_DEBUG
    LOGD("[PlayYuv] initCompose Src Size (%d,%d)",w,h);
    LOGD("[PlayYuv] initCompose center_all_1 (%f,%f) rad_all_1 %f",center_all_1.x, center_all_1.y, rad_all_1);
    LOGD("[PlayYuv] initCompose center_all_2 (%f,%f) rad_all_2 %f",center_all_2.x, center_all_2.y, rad_all_2);
#endif
    Generate_fusion_area_init_YUV420SP(
            Size(w, h),
            center_all_1, center_all_2,          //两个镜头中心参数
            rad_all_1, rad_all_2,                //镜头有效区域半径参数
            RotationMatrix, TMatrix, pdK,        //3D 点标定外参
            &mapY_x, &mapY_y,                    //imag_0 经纬展开 map Y
            &mapUV_x, &mapUV_y,                  //imag_0 经纬展开 map Y
            &im, &m_uv);                         //融合区 Mark
}

void PlayYuv::compose(AVFrame *frame) {
    //Mat y = Mat(frame->height, frame->width, CV_8UC1, frame->data[0]);
    //Mat u = Mat(frame->height / 2, frame->width / 2, CV_8UC1, frame->data[1]);
    //Mat v = Mat(frame->height / 2, frame->width / 2, CV_8UC1, frame->data[2]);
    //double alpha1Y;
    //double alpha2Y;//输出两个半球亮度调整的值
    Mat src_Y420_y = Mat(frame->height, frame->width, CV_8UC1, frame->data[0]);
    Mat src_Y420_uv = Mat(frame->height, (frame->width / 2), CV_8UC1, frame->data[1]);
    Generate_fusion_area_YUV420P(
            Size(frame->width, frame->height),
            src_Y420_y,
            src_Y420_uv,
            mapY_x, mapY_y,           //imag_0 经纬展开 map Y
            mapUV_x, mapUV_y,         //imag_0 经纬展开 map Y
            im, m_uv,
            &light1y, &light2y,
            &out_y, &out_u, &out_v);
#if GL_PLAY_DEBUG
    imwrite("/storage/emulated/0/Movies/out_y.jpg", out_y );
    imwrite("/storage/emulated/0/Movies/out_u.jpg", out_u );
    imwrite("/storage/emulated/0/Movies/out_v.jpg", out_v );
#endif
}