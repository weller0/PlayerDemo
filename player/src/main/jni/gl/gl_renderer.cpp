#include "gl/gl_renderer.h"

GLRenderer::GLRenderer(TransformBean *transformBean, SettingsBean *settingsBean) {
    LOGD("[GLRenderer] +");
    mSettingsBean = settingsBean;
    pBeanProcess = new GLBean();
    pBeanProcess->init();
    pBeanProcess->pTransformBean = transformBean;
    pBeanDisplay = new GLBean();
    pBeanDisplay->init();
}

GLRenderer::~GLRenderer() {
    glDeleteFramebuffers(1, &mFBOId);
    pBeanProcess->clear();
    delete pBeanProcess;
    pBeanDisplay->clear();
    delete pBeanDisplay;
    LOGD("[GLRenderer] -");
}

GLint GLRenderer::onSurfaceCreated() {
    LOGD("[GLRenderer:onSurfaceCreated]");
    prepareProcessBuffer();
    prepareDisplayBuffer();
    loadShader();

    // 创建纹理ID
    GLuint textureId[] = {0, 0};
    glGenTextures(2, textureId);
    pBeanProcess->mTextureId = textureId[0];
    pBeanDisplay->mTextureId = textureId[1];

    // 创建VAO、VBO
    GLuint size = pBeanProcess->pVertexBuffer->getSize();
    pBeanProcess->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanProcess->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    // 获取顶点数组对象VAO(Vertex Array Object)和顶点缓冲对象VBO(Vertex Buffer Objects)
    glGenVertexArrays(size, pBeanProcess->pVAO);
    glGenBuffers(size * 2, pBeanProcess->pVBO);

    size = pBeanDisplay->pVertexBuffer->getSize();
    pBeanDisplay->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanDisplay->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    glGenVertexArrays(size, pBeanDisplay->pVAO);
    glGenBuffers(size * 2, pBeanDisplay->pVBO);

    return pBeanProcess->mTextureId;
}

void GLRenderer::onSurfaceChanged(GLuint w, GLuint h) {
    LOGD("[GLRenderer:onSurfaceChanged]view size(%d, %d)", w, h);
    mWindowWidth = w;
    mWindowHeight = h;
    pBeanProcess->pMatrix->perspective(45, (GLfloat) w / (GLfloat) h, 0.1, 100);
    pBeanProcess->pMatrix->lookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
    glViewport(0, 0, w, h);
    configTexture(mWindowWidth, mWindowHeight);

    prepareFBO();
}

void GLRenderer::onDrawFrame(Bitmap *bmp) {
    updateBuffer(pBeanProcess);
    updateBuffer(pBeanDisplay);
    prepareDraw(bmp);

    glBindFramebuffer(GL_FRAMEBUFFER, mFBOId);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(pBeanProcess);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    draw(pBeanDisplay);
}

void GLRenderer::loadShader() {
    pBeanDisplay->mProgramHandle = createProgram(gRectVertexShader, gRectFragmentShader);
    LOGD("[GLRenderer:loadShader]pBeanDisplay->mProgramHandle=%d", pBeanDisplay->mProgramHandle);
}

void GLRenderer::configTexture(GLuint w, GLuint h) {
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
    LOGD("[GLRenderer:prepareDisplayBuffer]");
    pBeanDisplay->pTextureBuffer->updateBuffer((GLfloat *) rectTexture, sizeof(rectTexture),
                                               sizeof(rectTexture[0]), 2);
    pBeanDisplay->pVertexBuffer->updateBuffer((GLfloat *) rectVertex, sizeof(rectVertex),
                                              sizeof(rectVertex[0]), 3);
    pBeanDisplay->bUpdateBuffer = GL_TRUE;
}

void GLRenderer::prepareProcessBuffer() {
}

GLboolean GLRenderer::onSettingsChanged(GLuint sm, GLuint rr, GLuint cs) {
    GLboolean result = GL_FALSE;
    if (sm != mSettingsBean->mShowMode) {
        LOGD("[Picture:onSettingsChanged]mShowMode is changed");
        prepareProcessBuffer();
        prepareDisplayBuffer();
        result = GL_TRUE;
    }
    if (rr != mSettingsBean->mResolutionRatio) {
        LOGD("[Picture:onSettingsChanged]mResolutionRatio is changed");
    }
    if (cs != mSettingsBean->mCtrlStyle) {
        LOGD("[Picture:onSettingsChanged]mCtrlStyle is changed");
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

void GLRenderer::prepareFBO() {
    glGenFramebuffers(1, &mFBOId);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBOId);
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
        LOGE("[GLRenderer:prepareFBO] glCheckFramebufferStatus is fail(0x%x)", result);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::prepareDraw(Bitmap *bmp) {
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
        if (glBean->mProjectionHandle != 0) {
            glUniformMatrix4fv(glBean->mProjectionHandle, 1, GL_FALSE,
                               glBean->pMatrix->getProjectionMatrix());
        }
        if (glBean->mCameraHandle != 0) {
            glUniformMatrix4fv(glBean->mCameraHandle, 1, GL_FALSE,
                               glBean->pMatrix->getCameraMatrix());
        }
        if (glBean->mTransformHandle != 0) {
            glUniformMatrix4fv(glBean->mTransformHandle, 1, GL_FALSE,
                               glBean->pMatrix->getTransformMatrix());
        }
        if (glBean->mLightHandle != 0) {
            glUniform3f(glBean->mLightHandle, 1, 1, 1);
        }
        glBindTexture(glBean->eTextureTarget, glBean->mTextureId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, glBean->pVertexBuffer->getBuffer(i)->pointSize);
        // 解绑VAO
        glBindVertexArray(0);
        checkGLError("draw glBindVertexArray -");
    }
}