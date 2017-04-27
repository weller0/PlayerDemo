#include "gl/gl_renderer.h"
#include "gl_renderer.h"

GLRenderer::GLRenderer(TransformBean *transformBean, SettingsBean *settingsBean) {
    LOGI("[GLRenderer] +");
    mSettingsBean = settingsBean;
    pBeanOriginal = new GLBean();
    pBeanOriginal->init();
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
    pBeanOriginal->clear();
    delete pBeanOriginal;
    LOGI("[GLRenderer] -");
}

GLint GLRenderer::onSurfaceCreated() {
    LOGI("[GLRenderer:onSurfaceCreated]");
    prepareOriginalBuffer();
    prepareProcessBuffer();
    prepareDisplayBuffer();
    loadShader();

    // 创建纹理ID
    GLuint textureId[] = {0, 0, 0, 0};
    glGenTextures(4, textureId);
    pBeanOriginal->mTextureId = textureId[0];
    pBeanProcess->mTextureId = textureId[1];
    pBeanProcess->mComposeTextureId = textureId[2];
    pBeanDisplay->mTextureId = textureId[3];

    // 创建VAO、VBO
    GLuint size = pBeanOriginal->pVertexBuffer->getSize();
    pBeanOriginal->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanOriginal->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    // 获取顶点数组对象VAO(Vertex Array Object)和顶点缓冲对象VBO(Vertex Buffer Objects)
    glGenVertexArrays(size, pBeanOriginal->pVAO);
    glGenBuffers(size * 2, pBeanOriginal->pVBO);

    size = pBeanProcess->pVertexBuffer->getSize();
    pBeanProcess->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanProcess->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    glGenVertexArrays(size, pBeanProcess->pVAO);
    glGenBuffers(size * 2, pBeanProcess->pVBO);

    size = pBeanDisplay->pVertexBuffer->getSize();
    pBeanDisplay->pVAO = (GLuint *) malloc(size * sizeof(GLuint));
    pBeanDisplay->pVBO = (GLuint *) malloc(size * 2 * sizeof(GLuint));
    glGenVertexArrays(size, pBeanDisplay->pVAO);
    glGenBuffers(size * 2, pBeanDisplay->pVBO);

    return pBeanOriginal->mTextureId;
}

void GLRenderer::onSurfaceChanged(GLuint w, GLuint h) {
    LOGI("[GLRenderer:onSurfaceChanged]view size(%d, %d)", w, h);
    mWindowWidth = w;
    mWindowHeight = h;
    pBeanProcess->pMatrix->perspective(45, (GLfloat) w / (GLfloat) h, 0.1, 100);
    pBeanProcess->pMatrix->lookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
    glViewport(0, 0, w, h);
    configTexture(mWindowWidth, mWindowHeight);

    prepareProcessFBO();
    prepareDisplayFBO();
}

void GLRenderer::onDrawFrame(Bitmap *bmp, GLfloat r) {
    if(asp != r){

    }
    asp = r;
    updateBuffer(pBeanOriginal);
    updateBuffer(pBeanProcess);
    updateBuffer(pBeanDisplay);
    prepareDraw(bmp);

    glBindFramebuffer(GL_FRAMEBUFFER, mProcessFBOId);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(pBeanOriginal);

    glBindFramebuffer(GL_FRAMEBUFFER, mDisplayFBOId);
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
    pBeanProcess->mProgramHandle = createProgram(gOriVertexShader, gOriFragmentShader);
    // 获取投影、Camera、变换句柄
    pBeanProcess->mProjectionHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                           "projection");
    pBeanProcess->mCameraHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                       "camera");
    pBeanProcess->mTransformHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                          "transform");
    pBeanProcess->mLightHandle = glGetUniformLocation(pBeanProcess->mProgramHandle,
                                                      "light");
    pBeanDisplay->mProgramHandle = createProgram(gRectVertexShader, gRectFragmentShader);
    LOGI("[GLRenderer:loadShader]pBeanDisplay->mProgramHandle=%d", pBeanDisplay->mProgramHandle);
    LOGI("[GLRenderer:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);
}

void GLRenderer::configTexture(GLuint w, GLuint h) {
    // 绑定纹理,之后任何的纹理指令都可以配置当前绑定的纹理
    glBindTexture(pBeanOriginal->eTextureTarget, pBeanOriginal->mTextureId);
    // 纹理过滤
    // GL_NEAREST 当前像素值
    // GL_LINEAR 当前像素附近颜色的混合色
    glTexParameteri(pBeanOriginal->eTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(pBeanOriginal->eTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理剩余位置显示处理
    glTexParameteri(pBeanOriginal->eTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(pBeanOriginal->eTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
//    GLfloat *rect1 = (GLfloat *)malloc(sizeof(rectVertex1));
//    for (int i = 0; i < sizeof(rectVertex1) / sizeof(GLfloat); i += 3) {
//        rect1[i] = asp * ((GLfloat *)rectVertex1)[i];
//        rect1[i+1] = ((GLfloat *)rectVertex1)[i + 1];
//        rect1[i+2] = ((GLfloat *)rectVertex1)[i + 2];
//    }
    pBeanDisplay->pTextureBuffer->updateBuffer((GLfloat *) rectTexture, sizeof(rectTexture),
                                               sizeof(rectTexture[0]), 2);
    pBeanDisplay->pVertexBuffer->updateBuffer((GLfloat *) rectVertex, sizeof(rectVertex),
                                              sizeof(rectVertex[0]), 3);
    pBeanDisplay->bUpdateBuffer = GL_TRUE;
}

void GLRenderer::prepareOriginalBuffer() {
    LOGI("[GLRenderer:prepareOriginalBuffer]");
    pBeanOriginal->pTextureBuffer->updateBuffer((GLfloat *) rectTexture, sizeof(rectTexture),
                                                sizeof(rectTexture[0]), 2);
    pBeanOriginal->pVertexBuffer->updateBuffer((GLfloat *) rectVertex1, sizeof(rectVertex1),
                                               sizeof(rectVertex1[0]), 3);
    pBeanOriginal->bUpdateBuffer = GL_TRUE;
}

void GLRenderer::prepareProcessBuffer() {
    LOGI("[GLRenderer:prepareProcessBuffer]");
    GLuint totalSize = 0;
    GLuint unitSize = 0;
    GLuint bufSize = 0;
    File *file = new File((char *) "/storage/emulated/0/Movies/texcoord_buffer_1");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *textureBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(textureBuffer, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pTextureBuffer->updateBuffer(textureBuffer, totalSize * sizeof(GLfloat),
                                                   unitSize * sizeof(GLfloat), 2);
    } else {
        pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) videoTexture, sizeof(videoTexture),
                                                   sizeof(videoTexture[0]), 2);
    }
    delete file;

    totalSize = 0;
    unitSize = 0;
    bufSize = 0;
    file = new File((char *) "/storage/emulated/0/Movies/vertex_buffer_1");
    file->getBufferSize(&totalSize, &unitSize);
    GLfloat *vertexBuffer = (GLfloat *) malloc(totalSize * sizeof(GLfloat));
    file->getBuffer(vertexBuffer, &totalSize, &unitSize, &bufSize);
    if (bufSize == totalSize && bufSize > 4) {
        pBeanProcess->pVertexBuffer->updateBuffer(vertexBuffer, totalSize * sizeof(GLfloat),
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

void GLRenderer::prepareProcessFBO() {
    glGenFramebuffers(1, &mProcessFBOId);
    glBindFramebuffer(GL_FRAMEBUFFER, mProcessFBOId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pBeanProcess->eTextureTarget,
                           pBeanProcess->mTextureId, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("[GLRenderer:prepareProcessFBO] glCheckFramebufferStatus is fail(0x%x)", result);
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
        glBindTexture(glBean->eTextureTarget, glBean->mTextureId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, glBean->pVertexBuffer->getBuffer(i)->pointSize);
        // 解绑VAO
        glBindVertexArray(0);
        checkGLError("draw glBindVertexArray -");
    }
}