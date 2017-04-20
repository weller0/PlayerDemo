#include "gl/gl_picture.h"
#include "opencv2/opencv.hpp"
#include "compose/Generate_fusion_area.h"

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
    LOGI("[Picture:loadShader]pBeanProcess->mProgramHandle=%d", pBeanProcess->mProgramHandle);

}

void Picture::prepareProcessBuffer() {
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
        pBeanProcess->pTextureBuffer->updateBuffer((GLfloat *) picTexture, sizeof(picTexture),
                                                   sizeof(picTexture[0]), 2);
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
        pBeanProcess->pVertexBuffer->updateBuffer((GLfloat *) picVertex, sizeof(picVertex),
                                                  sizeof(picVertex[0]), 3);
    }
    delete file;

    pBeanProcess->bUpdateBuffer = GL_TRUE;
}

void Picture::prepareDraw(Bitmap *bmp) {
    if (bFirstFrame && bmp != NULL) {
        bFirstFrame = GL_FALSE;

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
        rad_all_1 = (float) parameter[2];
        center_all_2 = Point2f((float) parameter[3], (float) parameter[4]);
        rad_all_2 = (float) parameter[5];
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

        //初始化参数
        Mat imapx_roi0, imapy_roi0;     //imag_0 经纬展开 map
        Mat imapx_roi1, imapy_roi1;     //imag_1 经纬展开 map
        Mat im;

        GLuint64 lastTime = getCurrentTimeMs();
        //融合参数初始化，运行一次
        Generate_fusion_area_init(
                center_all_1, center_all_2,          //两个镜头中心参数
                rad_all_1, rad_all_2,                //镜头有效区域半径参数
                RotationMatrix, TMatrix, pdK,        //3D 点标定外参
                &imapx_roi0,
                &imapy_roi0,                         //imag_0 经纬展开 map
                &imapx_roi1,
                &imapy_roi1,                         //imag_1 经纬展开 map
                &im);                                //融合区 Mark
        LOGD("[Picture:Generate_fusion_area_init] %dms", (GLuint)(getCurrentTimeMs() - lastTime));

        lastTime = getCurrentTimeMs();
        //加载图片
        Mat imageA = imread("/storage/emulated/0/Movies/image_a.jpg");
        if (imageA.empty()) {
            LOGE("[Picture:prepareDraw]Can not load /storage/emulated/0/Movies/image_a.jpg");
            return;
        }
        Mat imageB = imread("/storage/emulated/0/Movies/image_b.jpg");
        if (imageA.empty()) {
            LOGE("[Picture:prepareDraw]Can not load /storage/emulated/0/Movies/image_b.jpg");
            return;
        }
        LOGD("[Picture:imread] %dms", (GLuint)(getCurrentTimeMs() - lastTime));
        LOGD("imageA (%d, %d), imageB (%d, %d)",
             imageA.cols, imageA.rows, imageB.cols, imageB.rows);

        lastTime = getCurrentTimeMs();
        //生成图片
        Mat img_out;
        Generate_fusion_area(imageA, imageB,                //输入原图片
                             imapx_roi0,
                             imapy_roi0,                    //imageA 经纬展开 map
                             imapx_roi1,
                             imapy_roi1,                    //imageB 经纬展开 map
                             im,                            //融合区 Mark
                             &img_out);
        LOGD("[Picture:Generate_fusion_area] %dms", (GLuint)(getCurrentTimeMs() - lastTime));
        LOGD("img_out (%d, %d)", img_out.cols, img_out.rows);
        glBindTexture(pBeanProcess->eTextureTarget, pBeanProcess->mTextureId);
        glTexImage2D(pBeanProcess->eTextureTarget, 0, GL_RGB,
                     img_out.cols, img_out.rows, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, img_out.data);
    }
}

