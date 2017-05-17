#include "gl/gl_renderer.h"

GLRenderer::GLRenderer(TransformBean *transformBean, SettingsBean *settingsBean) {
    LOGI("[GLRenderer] +");
    mSettingsBean = settingsBean;
    pBeanProcess = new GLBean();
    pBeanProcess->init();
    pBeanProcess->pTransformBean = transformBean;
    pBeanDisplay = new GLBean();
    pBeanDisplay->init();
}

GLRenderer::~GLRenderer() {
    glDeleteFramebuffers(1, &mDisplayFBOId);
    pBeanProcess->clear();
    delete pBeanProcess;
    pBeanDisplay->clear();
    delete pBeanDisplay;
    LOGI("[GLRenderer] -");
}

GLint GLRenderer::onSurfaceCreated() {
    LOGI("[GLRenderer:onSurfaceCreated]");
    prepareProcessBuffer();
    prepareDisplayBuffer();
    loadShader();

    // 创建纹理ID
    GLuint textureId[] = {0, 0, 0};
    glGenTextures(3, textureId);
    pBeanProcess->mTextureId = textureId[0];
    pBeanProcess->mComposeTextureId = textureId[1];
    pBeanDisplay->mTextureId = textureId[2];

    // 创建VAO、VBO
    GLuint size = pBeanProcess->pVertexBuffer->getSize();
    pBeanProcess->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanProcess->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    glGenVertexArrays(size, pBeanProcess->pVAO);
    glGenBuffers(size * 2, pBeanProcess->pVBO);

    size = pBeanDisplay->pVertexBuffer->getSize();
    pBeanDisplay->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanDisplay->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    glGenVertexArrays(size, pBeanDisplay->pVAO);
    glGenBuffers(size * 2, pBeanDisplay->pVBO);

    return updateTextureAuto();
}

void GLRenderer::onSurfaceChanged(GLuint w, GLuint h) {
    LOGI("[GLRenderer:onSurfaceChanged]view size(%d, %d)", w, h);
    mWindowWidth = w;
    mWindowHeight = h;
    pBeanProcess->pMatrix->perspective(45, (GLfloat) w / (GLfloat) h, 0.1, 100);
    pBeanProcess->pMatrix->lookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
    glViewport(0, 0, w, h);
    configTexture(mWindowWidth, mWindowHeight);
    prepareDisplayFBO();
}

void GLRenderer::onDrawFrame(Bitmap *bmp) {
    updateBuffer(pBeanProcess);
    updateBuffer(pBeanDisplay);
    if (!prepareDraw(bmp)) return;

    glBindFramebuffer(GL_FRAMEBUFFER, mDisplayFBOId);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (useYUVDraw()) {
        drawForYUV(pBeanProcess);
    } else {
        draw(pBeanProcess);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    draw(pBeanDisplay);
}

GLuint GLRenderer::updateTextureAuto() {
    return pBeanProcess->mTextureId;
}

void GLRenderer::drawForYUV(GLBean *glBean) {

}

GLboolean GLRenderer::useYUVDraw() {
    return GL_FALSE;
}

//void GLRenderer::prepareComposeTexture() {
//    glReadPixels(0, 0, mWindowWidth, mWindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, pComposeData);
//
//    if (isFirstFrame) {
//        isFirstFrame = GL_FALSE;
//        initCompose(mWindowWidth, mWindowHeight);
//        compose(mWindowWidth, mWindowHeight, pComposeData);
//        glBindTexture(GL_TEXTURE_2D, pBeanProcess->mComposeTextureId);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
//                     img_out.cols, img_out.rows, 0,
//                     GL_RGBA, GL_UNSIGNED_BYTE, img_out.data);
//    } else {
//        compose(mWindowWidth, mWindowHeight, pComposeData);
//        glBindTexture(GL_TEXTURE_2D, pBeanProcess->mComposeTextureId);
//        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
//                        img_out.cols, img_out.rows,
//                        GL_RGBA, GL_UNSIGNED_BYTE, img_out.data);
//    }
//}
//
//void GLRenderer::initCompose(GLuint w, GLuint h) {
//    // init
//    //需要参数，从文件读取
//    GLuint totalSize = 0;
//    GLuint unitSize = 0;
//    GLuint bufSize = 0;
//    File *file = new File((char *) "/storage/emulated/0/Movies/parameter");
//    file->getBufferSize(&totalSize, &unitSize);
//    LOGD("[File]parameter size %d, %d", totalSize, unitSize);
//    double *parameter = (double *) malloc(totalSize * sizeof(double));
//    file->getBuffer(parameter, &totalSize, &unitSize, &bufSize);
//
//
//    //读取保存的参数
//    Point2f center_all_1, center_all_2;        //两个镜头中心参数
//    float rad_all_1, rad_all_2;                //镜头有效区域半径参数
//    Mat RotationMatrix(3, 3, CV_64FC1);        //3D 点标定外参 旋转
//    Vec3d TMatrix;                             //3D 点标定外参 平移
//    double pdK;                                //3D 点标定外参 缩放
//
//    center_all_1 = Point2f((float) parameter[0], (float) parameter[1]);
//    rad_all_1 = (float) parameter[2];
//    center_all_2 = Point2f((float) parameter[3], (float) parameter[4]);
//    rad_all_2 = (float) parameter[5];
//    RotationMatrix.at<double>(0, 0) = parameter[6];
//    RotationMatrix.at<double>(0, 1) = parameter[7];
//    RotationMatrix.at<double>(0, 2) = parameter[8];
//    RotationMatrix.at<double>(1, 0) = parameter[9];
//    RotationMatrix.at<double>(1, 1) = parameter[10];
//    RotationMatrix.at<double>(1, 2) = parameter[11];
//    RotationMatrix.at<double>(2, 0) = parameter[12];
//    RotationMatrix.at<double>(2, 1) = parameter[13];
//    RotationMatrix.at<double>(2, 2) = parameter[14];
//    TMatrix[0] = parameter[15];
//    TMatrix[1] = parameter[16];
//    TMatrix[2] = parameter[17];
//    pdK = parameter[18];
//
//    Generate_fusion_area_init(
//            Size(w, h),
//            center_all_1, center_all_2,          //两个镜头中心参数
//            rad_all_1, rad_all_2,                //镜头有效区域半径参数
//            RotationMatrix, TMatrix, pdK,        //3D 点标定外参
//            &imapx_roi0,
//            &imapy_roi0,                         //imag_0 经纬展开 map
//            &imapx_roi1,
//            &imapy_roi1,                         //imag_1 经纬展开 map
//            &im);                                //融合区 Mark
//}
//
//void GLRenderer::compose(GLuint w, GLuint h, GLubyte *buffer) {
//    //加载图片
//    Mat imageA = Mat(w, h, CV_8UC3, buffer);
//    if (imageA.empty()) {
//        LOGE("[Picture:prepareDraw]Can not load image.");
//        return;
//    }
//    LOGD("imageA (%d, %d)", imageA.cols, imageA.rows);
//    GLuint64 lastTime = getCurrentTimeMs();
//    //生成图片
//    Generate_fusion_area(
//            Size(imageA.cols, imageA.rows),
//            imageA,                //输入原图片
//            imapx_roi0,
//            imapy_roi0,                    //imageA 经纬展开 map
//            imapx_roi1,
//            imapy_roi1,                    //imageB 经纬展开 map
//            im,                            //融合区 Mark
//            &img_out);
//    LOGD("[Picture:Generate_fusion_area] %dms", (GLuint) (getCurrentTimeMs() - lastTime));
//    LOGD("img_out (%d, %d)", img_out.cols, img_out.rows);
//}

void GLRenderer::loadShader() {
    pBeanDisplay->mProgramHandle = createProgram(gRectVertexShader, gRectFragmentShader);
    LOGI("[GLRenderer:loadShader]pBeanDisplay->mProgramHandle=%d", pBeanDisplay->mProgramHandle);
    LOGI("[GLRenderer:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

void GLRenderer::configTexture(GLuint w, GLuint h) {
    // 绑定纹理,之后任何的纹理指令都可以配置当前绑定的纹理
    glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mComposeTextureId);
    // 纹理过滤
    // GL_NEAREST 当前像素值
    // GL_LINEAR 当前像素附近颜色的混合色
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理剩余位置显示处理
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 绑定纹理,之后任何的纹理指令都可以配置当前绑定的纹理
    glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
    // 纹理过滤
    // GL_NEAREST 当前像素值
    // GL_LINEAR 当前像素附近颜色的混合色
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理剩余位置显示处理
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(pBeanProcess->eTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(pBeanProcess->eTextureTarget, 0, GL_RGBA,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(pBeanDisplay->eTextureTarget, pBeanDisplay->mTextureId);
    // 纹理过滤
    // GL_NEAREST 当前像素值
    // GL_LINEAR 当前像素附近颜色的混合色
    glTexParameteri(pBeanDisplay->eTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(pBeanDisplay->eTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理剩余位置显示处理
    glTexParameteri(pBeanDisplay->eTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(pBeanDisplay->eTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(pBeanDisplay->eTextureTarget, 0, GL_RGBA,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void GLRenderer::prepareDisplayBuffer() {
    LOGI("[GLRenderer:prepareDisplayBuffer]");
    pBeanDisplay->pTextureBuffer->updateBuffer((GLfloat *) rectTexture, sizeof(rectTexture),
                                               sizeof(rectTexture[0]), 2);
    pBeanDisplay->pVertexBuffer->updateBuffer((GLfloat *) rectVertex, sizeof(rectVertex),
                                              sizeof(rectVertex[0]), 3);
    pBeanDisplay->bUpdateBuffer = GL_TRUE;
}

void GLRenderer::prepareProcessBuffer() {
    LOGI("[GLRenderer:prepareProcessBuffer]");
    GLuint totalSize = 0;
    GLuint unitSize = 0;
    GLuint bufSize = 0;
    File *file = new File((char *) "/storage/emulated/0/Movies/texcoord_buffer_0");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *textureBuffer1 = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(textureBuffer1, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pTextureBuffer->updateBuffer(textureBuffer1, totalSize * sizeof(GLfloat),
                                                   unitSize * sizeof(GLfloat), 2);
    } else {
        pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) videoTexture, sizeof(videoTexture),
                                                   sizeof(videoTexture[0]), 2);
    }
    delete file;

    totalSize = 0;
    unitSize = 0;
    bufSize = 0;
    file = new File((char *) "/storage/emulated/0/Movies/texcoord_buffer_0");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *textureBuffer2 = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(textureBuffer2, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pTextureBuffer->add(textureBuffer2, totalSize * sizeof(GLfloat),
                                          unitSize * sizeof(GLfloat), 2);
    } else {
        pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) videoTexture, sizeof(videoTexture),
                                                   sizeof(videoTexture[0]), 2);
    }
    delete file;


    totalSize = 0;
    unitSize = 0;
    bufSize = 0;
    file = new File((char *) "/storage/emulated/0/Movies/vertex_buffer_0");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *vertexBuffer1 = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(vertexBuffer1, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pVertexBuffer->updateBuffer(vertexBuffer1, totalSize * sizeof(GLfloat),
                                                  unitSize * sizeof(GLfloat), 3);
    } else {
        pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) videoVertex, sizeof(videoVertex),
                                                  sizeof(videoVertex[0]), 3);
    }
    delete file;

    totalSize = 0;
    unitSize = 0;
    bufSize = 0;
    file = new File((char *) "/storage/emulated/0/Movies/vertex_buffer_0");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *vertexBuffer2 = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(vertexBuffer2, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pVertexBuffer->add(vertexBuffer2, totalSize * sizeof(GLfloat),
                                         unitSize * sizeof(GLfloat), 3);
    } else {
        pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) videoVertex, sizeof(videoVertex),
                                                  sizeof(videoVertex[0]), 3);
    }
    delete file;

    pBeanProcess->bUpdateBuffer = GL_TRUE;
}

GLboolean GLRenderer::onSettingsChanged(GLuint sm, GLuint rr, GLuint cs) {
    GLboolean result = GL_FALSE;
    if (sm != mSettingsBean->mShowMode) {
        LOGI("[Picture:onSettingsChanged]mShowMode is changed");
        prepareProcessBuffer();
//        prepareDisplayBuffer();
        result = GL_TRUE;
    }
    if (rr != mSettingsBean->mResolutionRatio) {
        LOGI("[Picture:onSettingsChanged]mResolutionRatio is changed");
    }
    if (cs != mSettingsBean->mCtrlStyle) {
        LOGI("[Picture:onSettingsChanged]mCtrlStyle is changed");
    }
    return result;
}

void GLRenderer::updateBuffer(GLBean *glBean) {
    if (glBean->bUpdateBuffer) {
        glBean->bUpdateBuffer = GL_FALSE;
        for (GLuint i = 0; i < glBean->pVertexBuffer->getSize(); i++) {
            // 绑定VAO，绑定之后设置顶点坐标相关的值
            glBindVertexArray(glBean->pVAO[i]);
            checkGLError("updateBuffer glBindVertexArray +");
            // 绑定顶点坐标并设置顶点坐标
            glBindBuffer(GL_ARRAY_BUFFER, glBean->pVBO[i * 2 + 0]);
            glBufferData(GL_ARRAY_BUFFER, glBean->pVertexBuffer->getBuffer(i)->bufferSize,
                         glBean->pVertexBuffer->getBuffer(i)->buffer, GL_DYNAMIC_DRAW);
            // 设置顶点属性指针
            glVertexAttribPointer(SHADER_IN_POSITION, 3, GL_FLOAT, GL_FALSE,
                                  3 * sizeof(GLfloat), (GLvoid *) 0);
            glEnableVertexAttribArray(SHADER_IN_POSITION);

            // 绑定纹理坐标并设置纹理坐标
            glBindBuffer(GL_ARRAY_BUFFER, glBean->pVBO[i * 2 + 1]);
            glBufferData(GL_ARRAY_BUFFER, glBean->pTextureBuffer->getBuffer(i)->bufferSize,
                         glBean->pTextureBuffer->getBuffer(i)->buffer, GL_DYNAMIC_DRAW);
            // 设置纹理属性指针
            glVertexAttribPointer(SHADER_IN_TEX_COORDS, 2, GL_FLOAT, GL_FALSE,
                                  2 * sizeof(GLfloat), (GLvoid *) 0);
            glEnableVertexAttribArray(SHADER_IN_TEX_COORDS);
            // 解绑VAO
            glBindVertexArray(0);
            checkGLError("updateBuffer glBindVertexArray -");
        }
    }
}

void GLRenderer::prepareDisplayFBO() {
    glGenFramebuffers(1, &mDisplayFBOId);
    glBindFramebuffer(GL_FRAMEBUFFER, mDisplayFBOId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pBeanDisplay->eTextureTarget,
                           pBeanDisplay->mTextureId, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("[GLRenderer:prepareDisplayFBO] glCheckFramebufferStatus is fail(0x%x)", result);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLboolean  GLRenderer::prepareDraw(Bitmap *bmp) {
    return GL_TRUE;
}

void GLRenderer::draw(GLBean *glBean) {
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
            glBindTexture(glBean->eTextureTarget, glBean->mTextureId);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, glBean->pVertexBuffer->getBuffer(i)->pointSize);
        // 解绑VAO
        glBindVertexArray(0);
        checkGLError("draw glBindVertexArray -");
    }
}