/*
 * mycv.cpp
 *
 *  Created on: 2017-2-15
 *      Author: lyz1900
 */

#include"mycv.h"

#define IWSHOW_DEBUG 0

/* 外切法找圆心半径************************************************************************************************/
void Circumscribe(Mat src, int T, Point2f *centerf, float *radiusf, int num) {
    Point2f centerf_in;
    Mat midImage;        //霍夫变换中间图
    Point center;        //霍夫变换圆形参数 圆心



    cvtColor(src, midImage, COLOR_BGR2GRAY);    //转成灰度图

    vector<int> row_max_D;

    // 扫描行列最大亮度差
    int rowNumber = midImage.rows;
    int colNumber = midImage.cols * midImage.channels();
    int up = 0, down = 0, left = 0, right = 0;
    //int T = 50;		//极亮度差阈值

    //扫描行极亮度差
    up = 0;    //初始值
    for (int i = 0; i < rowNumber; i++)            //行扫描，从上向下搜索
    {
        uchar *data = midImage.ptr<uchar>(i);    //获取第 i 行首地址
        uchar max = 0, min = 0;
        for (int j = 0; j < colNumber; j++)        //扫描第 i 行，找到最大值和最小值
        {
            uchar Y = data[j];
            if (Y > max) {
                max = Y;
            }
            if (Y < min) {
                min = Y;
            }
        }
        int YD = max - min;
        row_max_D.push_back(YD);
        if (up != 0 && YD > T) {
            if ((i - up) != 1)    //出现极亮度>T 的情况不连续，为躁点
            {
                up = i;            //更新 up 为新值
            }
            else {
                break;            //出现极亮度>T 的情况连续，为鱼眼边界
            }

        }
        if (YD > T) {
            up = i;
        }
    }

    //扫描行极亮度差
    down = rowNumber - 1;
    for (int i = rowNumber - 1; i > -1; i--)            //行扫描，从下向上搜索
    {
        uchar *data = midImage.ptr<uchar>(i);    //获取第 i 行首地址
        uchar max = 0, min = 0;
        for (int j = 0; j < colNumber; j++)        //扫描第 i 行，找到最大值和最小值
        {
            uchar Y = data[j];
            if (Y > max) {
                max = Y;
            }
            if (Y < min) {
                min = Y;
            }
        }
        int YD = max - min;
        row_max_D.push_back(YD);
        if (down != (rowNumber - 1) && YD > T) {
            if ((down - i) != 1)    //出现极亮度>T 的情况不连续，为躁点
            {
                down = i;            //更新 up 为新值
            }
            else {
                break;            //出现极亮度>T 的情况连续，为鱼眼边界
            }

        }
        if (YD > T) {
            down = i;
        }
    }

    //扫描列极亮度差
    for (int j = 0; j < colNumber; j++)        //列循环，从左到右搜索
    {
        uchar max = 0, min = 0;
        for (int i = 0; i < rowNumber; i++)            //列扫描
        {
            uchar *data = midImage.ptr<uchar>(i);    //获取第 i 行首地址
            uchar Y = data[j];                        //获取第 j 列 i 行
            if (Y > max) {
                max = Y;
            }
            if (Y < min) {
                min = Y;
            }
        }
        int YD = max - min;
        row_max_D.push_back(YD);
        if (left != 0 && YD > T) {
            if ((j - left) != 1)    //出现极亮度>T 的情况不连续，为躁点
            {
                left = j;            //更新 up 为新值
            }
            else {
                break;            //出现极亮度>T 的情况连续，为鱼眼边界
            }

        }
        if (YD > T) {
            left = j;
        }
    }

    //扫描列极亮度差
    for (int j = colNumber - 1; j > 0; j--)        //列循环，从左到右搜索
    {
        uchar max = 0, min = 0;
        for (int i = 0; i < rowNumber; i++)            //列扫描
        {
            uchar *data = midImage.ptr<uchar>(i);    //获取第 i 行首地址
            uchar Y = data[j];                        //获取第 j 列 i 行
            if (Y > max) {
                max = Y;
            }
            if (Y < min) {
                min = Y;
            }
        }
        int YD = max - min;
        row_max_D.push_back(YD);
        if (right != 0 && YD > T) {
            if ((right - j) != 1)    //出现极亮度>T 的情况不连续，为躁点
            {
                right = j;            //更新 up 为新值
            }
            else {
                break;            //出现极亮度>T 的情况连续，为鱼眼边界
            }

        }
        if (YD > T) {
            right = j;
        }
    }

    //找到外切四边形
    *centerf = Point2f(((float) left + ((float) right - (float) left) / 2.0),
                       ((float) up + ((float) down - (float) up) / 2.0));
    *radiusf = (((float) down - (float) up) + ((float) right - (float) left)) / 4.0;
#if IWSHOW_DEBUG
    int src_width = src.cols;
    int src_hight = src.rows;
    Mat src_clone = src.clone();
    char imanme[50];
    if (src_hight > 1080)
    {
        Mat src_resize;
        line(src_clone, Point(0, up), Point(src_width, up), Scalar(0, 255, 0), 3, 8);
        line(src_clone, Point(0, down), Point(src_width, down), Scalar(0, 255, 0), 3, 8);
        line(src_clone, Point(left, 0), Point(left, src_hight), Scalar(0, 255, 0), 3, 8);
        line(src_clone, Point(right, 0), Point(right, src_hight), Scalar(0, 255, 0), 3, 8);
        circle(src_clone, Point((left + (right - left) / 2), (up + (down - up) / 2)), 3, Scalar(0, 255, 0), 3, 8, 0);//绘制圆心
        circle(src_clone, Point((left + (right - left) / 2), (up + (down - up) / 2)), ((down - up) + (right - left)) / 4, Scalar(155, 50, 255), 3, 8, 0);//绘制圆边
        resize(src_clone, src_resize, Size(src.cols / 4, src.rows / 4), 0, 0, INTER_LINEAR);
        sprintf(imanme, "Circumscribe Src Resize %d", num);
        imshow(imanme, src_resize);
    }
    else
    {
        line(src_clone, Point(0, up), Point(src_width, up), Scalar(0, 255, 0), 1, 8);
        line(src_clone, Point(0, down), Point(src_width, down), Scalar(0, 255, 0), 1, 8);
        line(src_clone, Point(left, 0), Point(left, src_hight), Scalar(0, 255, 0), 1, 8);
        line(src_clone, Point(right, 0), Point(right, src_hight), Scalar(0, 255, 0), 1, 8);
        circle(src_clone, Point((left + (right - left) / 2), (up + (down - up) / 2)), 3, Scalar(0, 255, 0), -1, 8, 0);//绘制圆心
        circle(src_clone, Point((left + (right - left) / 2), (up + (down - up) / 2)), ((down - up) + (right - left)) / 4, Scalar(155, 50, 255), 2, 8, 0);//绘制圆边
        imshow("Circumscribe Src", src_clone);
    }
    waitKey();
#endif
}
/****************************************************************************************************************/
/*统计 yuv420p 图像数据 Y 通道图像亮度差*/
void YUV_match_avg_yuv420p(Mat src1, Mat src2,
                           double *alpha1Y, double *alpha2Y) {
    size_t rowNumber = src1.rows;
    size_t colNumber = src1.cols * src1.channels();

    unsigned int Y_sum_1 = 0, Y_sum_2 = 0;
    unsigned int Pix_num = 0;
    for (size_t i = 0; i < rowNumber; i++) {
        uchar *data1 = src1.ptr<uchar>(i);
        uchar *data2 = src2.ptr<uchar>(i);
        for (size_t j = 0; j < colNumber; j++) {
            Y_sum_1 += data1[j];
            Y_sum_2 += data2[j];
            Pix_num++;
        }
    }
    double Yaveg = ((double) Y_sum_1 + (double) Y_sum_2) / 2.0;
    double alphaY1 = Yaveg / (double) Y_sum_1;
    double alphaY2 = Yaveg / (double) Y_sum_2;
    *alpha1Y = alphaY1;
    *alpha2Y = alphaY2;
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_0_map_roi_yuv420sp(Size src_size, Size dest_size,
                                                     Point2f centerf_0, float radiusf_0,
                                                     Mat RotateMat, Vec3d TMatrix, double dbK,
                                                     Mat *mapx_ud, Mat *mapy_ud,
                                                     Mat *mapx_ud_2, Mat *mapy_ud_2,
                                                     float zoom) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;
    int start_hight = cvRound((0.25 + 0.75 * ANGLE) * (float) hight);
    int roi_hight = cvRound((1.0 - ANGLE) * (float) hight * 0.5);        //roi 区域的高度
    int stop_hight = start_hight + roi_hight;//cvRound((0.75 + 0.25 * ANGLE) * (float )hight);
    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);

    double *pRotateMat = RotateMat.ptr<double>(0);

    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (float) rows / (float) hight;    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X
            double x = ty * cos(P2PI * tx);
            double y = ty * sin(P2PI * tx);
            double x2y2 = (x * x + y * y);
            double z = 0;
            if (x2y2 <= 1) {
                z = sqrt(1 - x2y2);
            }
            double x1 = ((dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) +
                          TMatrix[0]) * radiusf_0 + centerf_0.x) * zoom;        //只适应了原图
            double y1 = ((dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) +
                          TMatrix[1]) * radiusf_0 + centerf_0.y) * zoom;
            //double z1 = (dbK*(pRotateMat[6]*x + pRotateMat[7]*y + pRotateMat[8]*z)+TMatrix[2]);
            mapx.at<float>(rows - start_hight, cols) = (x1 > (double) src_w) ? (float) src_w
                                                                             : (float) (x1);
            mapy.at<float>(rows - start_hight, cols) = (y1 > (double) src_h) ? (float) src_h
                                                                             : (float) (y1);
        }
    }
    Mat mapx_2(mapx.rows / 2, mapx.cols / 2, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy_2(mapy.rows / 2, mapy.cols / 2, CV_32FC1);
    for (rows = 0; rows < mapx.rows / 2; rows++)    //行循环1080 y
    {
        for (cols = 0; cols < mapx.cols / 2; cols++)    //列循环1920 x
        {
            mapx_2.at<float>(rows, cols) = mapx.at<float>(rows * 2, cols * 2) / 2;
            mapy_2.at<float>(rows, cols) = mapy.at<float>(rows * 2, cols * 2) / 2;
        }
    }
//    resize(mapx,mapx_2,Size(mapx.cols/2,mapx.rows/2),INTER_LINEAR );
//    resize(mapy,mapy_2,Size(mapy.cols/2,mapy.rows/2),INTER_LINEAR );
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
    *mapx_ud_2 = mapx_2.clone();
    *mapy_ud_2 = mapy_2.clone();
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_1_map_roi_yuv420sp(Size src_size, Size dest_size,
                                                     Point2f centerf_1, float radiusf_1,
                                                     Mat *mapx_ud, Mat *mapy_ud,
                                                     Mat *mapx_ud_2, Mat *mapy_ud_2, float zoom) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1 * zoom) / ((float) src_w);
    float dx = (centerf_1.x * zoom) / (float) src_w;
    float ky = (radiusf_1 * zoom) / ((float) src_h);
    float dy = (centerf_1.y * zoom) / (float) src_h;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;
    int start_hight = cvRound((1 - ANGLE) * (float) hight * 0.25);
    int roi_hight = cvRound((1.0 - ANGLE) * (float) hight * 0.5);        //roi 区域的高度
    int stop_hight = start_hight + roi_hight;//cvRound((1 - ANGLE) * (float )hight * 0.75);
    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;
    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (1.0 - ((float) rows / (float) hight));    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X

            double x = ty * cos(P2PI * tx) * kx + dx;
            double y = ty * sin(P2PI * tx) * ky + dy;

            mapx.at<float>(rows - start_hight, cols) = (float) (1.0 - x) * (float) src_w;
            mapy.at<float>(rows - start_hight, cols) = (float) (y) * (float) src_h;
        }
    }
    Mat mapx_2(mapx.rows / 2, mapx.cols / 2, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy_2(mapy.rows / 2, mapy.cols / 2, CV_32FC1);
    for (rows = 0; rows < mapx.rows / 2; rows++)    //行循环1080 y
    {
        for (cols = 0; cols < mapx.cols / 2; cols++)    //列循环1920 x
        {
            mapx_2.at<float>(rows, cols) = mapx.at<float>(rows * 2, cols * 2) / 2;
            mapy_2.at<float>(rows, cols) = mapy.at<float>(rows * 2, cols * 2) / 2;
        }
    }
//    resize(mapx,mapx_2,Size(mapx.cols/2,mapx.rows/2),INTER_LINEAR );
//    resize(mapy,mapy_2,Size(mapy.cols/2,mapy.rows/2),INTER_LINEAR );
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
    *mapx_ud_2 = mapx_2.clone();
    *mapy_ud_2 = mapy_2.clone();
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_0_map_roi(Size src_size, Size dest_size,
                                            Point2f centerf_0, float radiusf_0,
                                            Mat RotateMat, Vec3d TMatrix, double dbK,
                                            Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;
    int start_hight = cvRound((0.25 + 0.75 * ANGLE) * (float) hight);
    int roi_hight = cvRound((1.0 - ANGLE) * (float) hight * 0.5);        //roi 区域的高度
    int stop_hight = start_hight + roi_hight;//cvRound((0.75 + 0.25 * ANGLE) * (float )hight);
    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);

    double *pRotateMat = RotateMat.ptr<double>(0);

    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (float) rows / (float) hight;    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X
            double x = ty * cos(P2PI * tx);
            double y = ty * sin(P2PI * tx);
            double x2y2 = (x * x + y * y);
            double z = 0;
            if (x2y2 <= 1) {
                z = sqrt(1 - x2y2);
            }
            double x1 = (dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) +
                         TMatrix[0]) * radiusf_0 + centerf_0.x;
            double y1 = (dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) +
                         TMatrix[1]) * radiusf_0 + centerf_0.y;
            //double z1 = (dbK*(pRotateMat[6]*x + pRotateMat[7]*y + pRotateMat[8]*z)+TMatrix[2]);
            mapx.at<float>(rows - start_hight, cols) = (x1 > (double) src_w) ? (float) src_w
                                                                             : (float) (x1);
            mapy.at<float>(rows - start_hight, cols) = (y1 > (double) src_h) ? (float) src_h
                                                                             : (float) (y1);
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_1_map_roi(Size src_size, Size dest_size,
                                            Point2f centerf_1, float radiusf_1,
                                            Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1) / ((float) src_w);
    float dx = (centerf_1.x) / (float) src_w;
    float ky = (radiusf_1) / ((float) src_h);
    float dy = (centerf_1.y) / (float) src_h;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;
    int start_hight = cvRound((1 - ANGLE) * (float) hight * 0.25);
    int roi_hight = cvRound((1.0 - ANGLE) * (float) hight * 0.5);        //roi 区域的高度
    int stop_hight = start_hight + roi_hight;//cvRound((1 - ANGLE) * (float )hight * 0.75);
    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;

    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (1.0 - ((float) rows / (float) hight));    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X

            double x = ty * cos(P2PI * tx) * kx + dx;
            double y = ty * sin(P2PI * tx) * ky + dy;

            mapx.at<float>(rows - start_hight, cols) = (float) (1.0 - x) * (float) src_w;
            mapy.at<float>(rows - start_hight, cols) = (float) (y) * (float) src_h;
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 map****************************************************************************************/
void Generate_Equirectangular_src_0_map(Size src_size, Size dest_size,
                                        Point2f centerf_0, float radiusf_0,
                                        Mat RotateMat, Vec3d TMatrix, double dbK,
                                        Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;

    int stop_hight = cvRound((0.25 + 0.75 * ANGLE) * (float) hight) - 1;
    int roi_hight = cvRound((0.25 + 0.75 * ANGLE) * (float) hight) - 1;        //roi 区域的高度

    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;
    double *pRotateMat = RotateMat.ptr<double>(0);

    for (rows = 0; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (float) rows / (float) hight;    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X
            double x = ty * cos(P2PI * tx);
            double y = ty * sin(P2PI * tx);
            double x2y2 = (x * x + y * y);
            double z = 0;
            if (x2y2 <= 1) {
                z = sqrt(1 - x2y2);
            }
            P2.push_back(Point3f(x, y, z));
            double x1 = (dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) +
                         TMatrix[0]) * radiusf_0 + centerf_0.x;
            double y1 = (dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) +
                         TMatrix[1]) * radiusf_0 + centerf_0.y;
            //double z1 = (dbK*(pRotateMat[6]*x + pRotateMat[7]*y + pRotateMat[8]*z)+TMatrix[2]);
            mapx.at<float>(rows, cols) = (x1 > (double) src_w) ? (float) src_w : (float) (x1);
            mapy.at<float>(rows, cols) = (y1 > (double) src_h) ? (float) src_h : (float) (y1);
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 map****************************************************************************************/
void Generate_Equirectangular_src_1_map(Size src_size, Size dest_size,
                                        Point2f centerf_1, float radiusf_1,
                                        Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1) / ((float) src_w);
    float dx = (centerf_1.x) / (float) src_w;
    float ky = (radiusf_1) / ((float) src_h);
    float dy = (centerf_1.y) / (float) src_h;
    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;

    int start_hight = cvRound((1.0 - ANGLE) * (float) hight * 0.75) + 1;        //+1 修补浮点产生的误差
    int roi_hight = cvRound((0.25 + 0.75 * ANGLE) * (float) hight) - 1;        //roi 区域的高度

    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;


    for (rows = start_hight; rows < hight; rows++)    //行循环1080 y
    {
        float ty = (1.0 - ((float) rows / (float) hight));    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X

            double x = ty * cos(P2PI * tx) * kx + dx;
            double y = ty * sin(P2PI * tx) * ky + dy;

            mapx.at<float>(rows - start_hight, cols) = (float) (1.0 - x) * (float) src_w;
            mapy.at<float>(rows - start_hight, cols) = (float) (y) * (float) src_h;
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_0_map_roi_2(Size src_size, Size dest_size,
                                              Point2f centerf_0, float radiusf_0,
                                              Mat RotateMat, Vec3d TMatrix, double dbK,
                                              Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;
    int start_hight = cvRound(ANGLE * (float) hight);
    int roi_hight = hight - start_hight; //cvRound((1.0 - ANGLE) * (float )hight);		//roi 区域的高度
    int stop_hight = hight;//start_hight + roi_hight;//cvRound((0.75 + 0.25 * ANGLE) * (float )hight);
    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);

    double *pRotateMat = RotateMat.ptr<double>(0);

    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (float) rows / (float) hight;    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X
            double x = ty * cos(P2PI * tx);
            double y = ty * sin(P2PI * tx);
            double x2y2 = (x * x + y * y);
            double z = 0;
            if (x2y2 <= 1) {
                z = sqrt(1 - x2y2);
            }
            double x1 = (dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) +
                         TMatrix[0]) * radiusf_0 + centerf_0.x;
            double y1 = (dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) +
                         TMatrix[1]) * radiusf_0 + centerf_0.y;
            //double z1 = (dbK*(pRotateMat[6]*x + pRotateMat[7]*y + pRotateMat[8]*z)+TMatrix[2]);
            mapx.at<float>(rows - start_hight, cols) = (x1 > (double) src_w) ? (float) src_w
                                                                             : (float) (x1);
            mapy.at<float>(rows - start_hight, cols) = (y1 > (double) src_h) ? (float) src_h
                                                                             : (float) (y1);
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 map_roi*************************************************************************************/
void Generate_Equirectangular_src_1_map_roi_2(Size src_size, Size dest_size,
                                              Point2f centerf_1, float radiusf_1,
                                              Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1) / ((float) src_w);
    float dx = (centerf_1.x) / (float) src_w;
    float ky = (radiusf_1) / ((float) src_h);
    float dy = (centerf_1.y) / (float) src_h;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;

    int start_hight = 0;//cvRound((1 - ANGLE) * (float )hight * 0.25);
    int stop_hight = hight - cvRound(ANGLE *
                                     (float) hight);//start_hight + roi_hight;//cvRound((1 - ANGLE) * (float )hight * 0.75);
    int roi_hight = stop_hight;//cvRound((1.0 - ANGLE) * (float )hight * 0.5);		//roi 区域的高度

    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;

    for (rows = start_hight; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (1.0 - ((float) rows / (float) hight));    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X

            double x = ty * cos(P2PI * tx) * kx + dx;
            double y = ty * sin(P2PI * tx) * ky + dy;

            mapx.at<float>(rows - start_hight, cols) = (float) (1.0 - x) * (float) src_w;
            mapy.at<float>(rows - start_hight, cols) = (float) (y) * (float) src_h;
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 map****************************************************************************************/
void Generate_Equirectangular_src_0_map_2(Size src_size, Size dest_size,
                                          Point2f centerf_0, float radiusf_0,
                                          Mat RotateMat, Vec3d TMatrix, double dbK,
                                          Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;

    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;

    int stop_hight = cvRound(
            ANGLE * (float) hight);//cvRound((0.25 + 0.75 * ANGLE) * (float )hight)-1;
    int roi_hight = stop_hight;//cvRound((0.25 + 0.75 * ANGLE) * (float )hight)-1;		//roi 区域的高度

    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;
    double *pRotateMat = RotateMat.ptr<double>(0);

    for (rows = 0; rows < stop_hight; rows++)    //行循环1080 y
    {
        float ty = (float) rows / (float) hight;    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X
            double x = ty * cos(P2PI * tx);
            double y = ty * sin(P2PI * tx);
            double x2y2 = (x * x + y * y);
            double z = 0;
            if (x2y2 <= 1) {
                z = sqrt(1 - x2y2);
            }
            P2.push_back(Point3f(x, y, z));
            double x1 = (dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) +
                         TMatrix[0]) * radiusf_0 + centerf_0.x;
            double y1 = (dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) +
                         TMatrix[1]) * radiusf_0 + centerf_0.y;
            //double z1 = (dbK*(pRotateMat[6]*x + pRotateMat[7]*y + pRotateMat[8]*z)+TMatrix[2]);
            mapx.at<float>(rows, cols) = (x1 > (double) src_w) ? (float) src_w : (float) (x1);
            mapy.at<float>(rows, cols) = (y1 > (double) src_h) ? (float) src_h : (float) (y1);
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 map****************************************************************************************/
void Generate_Equirectangular_src_1_map_2(Size src_size, Size dest_size,
                                          Point2f centerf_1, float radiusf_1,
                                          Mat *mapx_ud, Mat *mapy_ud) {
    int width, hight;
    int cols, rows;        //循环变量

    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1) / ((float) src_w);
    float dx = (centerf_1.x) / (float) src_w;
    float ky = (radiusf_1) / ((float) src_h);
    float dy = (centerf_1.y) / (float) src_h;
    //半球投影大小
    width = dest_size.width;
    hight = dest_size.height;


    int roi_hight = cvRound(ANGLE *
                            (float) hight);//cvRound((0.25 + 0.75 * ANGLE) * (float )hight) - 1;		//roi 区域的高度
    int start_hight =
            hight - roi_hight;//cvRound((1.0 - ANGLE) * (float )hight * 0.75) + 1;		//+1 修补浮点产生的误差

    //生成的映射表
    Mat mapx(roi_hight, width, CV_32FC1);    //转成 remap 函数所需格式，并判断范围是否在规定图像范围内
    Mat mapy(roi_hight, width, CV_32FC1);
    vector<Point3d> P2;


    for (rows = start_hight; rows < hight; rows++)    //行循环1080 y
    {
        float ty = (1.0 - ((float) rows / (float) hight));    //归一化 Y
        for (cols = 0; cols < width; cols++)    //列循环1920 x
        {
            float tx = (float) cols / (float) width;        //归一化 X

            double x = ty * cos(P2PI * tx) * kx + dx;
            double y = ty * sin(P2PI * tx) * ky + dy;

            mapx.at<float>(rows - start_hight, cols) = (float) (1.0 - x) * (float) src_w;
            mapy.at<float>(rows - start_hight, cols) = (float) (y) * (float) src_h;
        }
    }
    *mapx_ud = mapx.clone();
    *mapy_ud = mapy.clone();
}
/****************************************************************************************************************/

/* SRC0 生成完整映射情况 坐标变换************************************************************************************/
void src_0_Equirectangular_coordinate_transformation(Point2f Texture_Coordinates,
                                                     Point2f *Image_Coordinates, Size src_size,
                                                     Point2f centerf_0, float radiusf_0,
                                                     Mat RotateMat, Vec3d TMatrix, double dbK) {
    double tx = (double) Texture_Coordinates.x;
    double ty = (double) Texture_Coordinates.y;

    if (tx > 1.0) {
        tx = 1.0;
    }
    else if (tx < 0) {
        tx = 0;
    }
    if (ty > 1.0) {
        ty = 1.0;
    }
    else if (tx < 0) {
        ty = 0;
    }
    double *pRotateMat = RotateMat.ptr<double>(0);
    double x = ty * cos(P2PI * tx);
    double y = ty * sin(P2PI * tx);
    double x2y2 = (x * x + y * y);
    double z = 0;
    if (x2y2 <= 1) {
        z = sqrt(1 - x2y2);
    }

    double x1 = (dbK * (pRotateMat[0] * x + pRotateMat[1] * y + pRotateMat[2] * z) + TMatrix[0]) *
                radiusf_0 + centerf_0.x;
    double y1 = (dbK * (pRotateMat[3] * x + pRotateMat[4] * y + pRotateMat[5] * z) + TMatrix[1]) *
                radiusf_0 + centerf_0.y;
    //double z1 = (dbK * (pRotateMat[6] * x + pRotateMat[7] * y + pRotateMat[8] * z) + TMatrix[2]);

    Image_Coordinates->x = (float) (x1 / (double) src_size.width);
    Image_Coordinates->y = (float) (y1 / (double) src_size.height);
}
/****************************************************************************************************************/

/* SRC1 生成完整映射情况 坐标变换************************************************************************************/
void src_1_Equirectangular_coordinate_transformation(Point2f Texture_Coordinates,
                                                     Point2f *Image_Coordinates, Size src_size,
                                                     Point2f centerf_1, float radiusf_1) {
    double tx = (double) Texture_Coordinates.x;
    double ty = 1.0 - (double) Texture_Coordinates.y;

    if (tx > 1.0) {
        tx = 1.0;
    }
    else if (tx < 0) {
        tx = 0;
    }
    if (ty > 1.0) {
        ty = 1.0;
    }
    else if (tx < 0) {
        ty = 0;
    }
    //原图大小
    int src_w = src_size.width;
    int src_h = src_size.height;
    //经纬图投影
    float kx = (radiusf_1) / ((float) src_w);
    float dx = (centerf_1.x) / (float) src_w;
    float ky = (radiusf_1) / ((float) src_h);
    float dy = (centerf_1.y) / (float) src_h;
    double x = ty * cos(P2PI * tx) * kx + dx;
    double y = ty * sin(P2PI * tx) * ky + dy;

    Image_Coordinates->x = (float) (1.0 - x);
    Image_Coordinates->y = (float) (y);
}
/****************************************************************************************************************/

/*获取亮度 K 值，转换为平均值***************************************************************************************/
void YUV_match_avg(Mat src1, Mat src2,
                   double *alpha1Y, double *alpha1U, double *alpha1V,
                   double *alpha2Y, double *alpha2U, double *alpha2V) {
    //转换成 HSV 空间
    Mat image1_YUV;
    Mat image2_YUV;
    cvtColor(src1, image1_YUV, COLOR_BGR2YUV);    //COLOR_BGR2YUV COLOR_BGR2HSV
    cvtColor(src2, image2_YUV, COLOR_BGR2YUV);

    size_t rowNumber = image1_YUV.rows;
    size_t colNumber = image1_YUV.cols * image1_YUV.channels();

    unsigned int Y_sum_1 = 0, Y_sum_2 = 0;
    unsigned int U_sum_1 = 0, U_sum_2 = 0;
    unsigned int V_sum_1 = 0, V_sum_2 = 0;
    unsigned int Pix_num = 0;
    for (size_t i = 0; i < rowNumber; i++) {
        uchar *data1 = image1_YUV.ptr<uchar>(i);
        uchar *data2 = image2_YUV.ptr<uchar>(i);
        for (size_t j = 0; j < colNumber; j += 3) {
            Y_sum_1 += data1[j];
            Y_sum_2 += data2[j];
            U_sum_1 += data1[j + 1];
            U_sum_2 += data2[j + 1];
            V_sum_1 += data1[j + 2];
            V_sum_2 += data2[j + 2];
            Pix_num++;
        }
    }

    double Yaveg = ((double) Y_sum_1 + (double) Y_sum_2) / 2.0;
    double Uaveg = ((double) U_sum_1 + (double) U_sum_2) / 2.0;
    double Vaveg = ((double) V_sum_1 + (double) V_sum_2) / 2.0;

    double alphaY1 = Yaveg / (double) Y_sum_1;
    double alphaU1 = Uaveg / (double) U_sum_1;
    double alphaV1 = Vaveg / (double) V_sum_1;

    double alphaY2 = Yaveg / (double) Y_sum_2;
    double alphaU2 = Uaveg / (double) U_sum_2;
    double alphaV2 = Vaveg / (double) V_sum_2;

    *alpha1Y = alphaY1;
    *alpha1U = alphaU1;
    *alpha1V = alphaV1;

    *alpha2Y = alphaY2;
    *alpha2U = alphaU2;
    *alpha2V = alphaV2;
}
/****************************************************************************************************************/

/*获取亮度 K 值****************************************************************************************************/
void YUV_match(Mat src1, Mat src2, int *Y21Y12,
               double *alpha21Y)//, double *alpha21U, double *alpha21V
{
    //转换成 HSV 空间
    Mat image1_YUV;
    Mat image2_YUV;
    cvtColor(src1, image1_YUV, COLOR_BGR2YUV);    //COLOR_BGR2YUV COLOR_BGR2HSV
    cvtColor(src2, image2_YUV, COLOR_BGR2YUV);

    size_t rowNumber = image1_YUV.rows;
    size_t colNumber = image1_YUV.cols * image1_YUV.channels();

    unsigned int Y_sum_1 = 0, Y_sum_2 = 0;
    for (size_t i = 0; i < rowNumber; i++) {
        uchar *data1 = image1_YUV.ptr<uchar>(i);
        uchar *data2 = image2_YUV.ptr<uchar>(i);
        for (size_t j = 0; j < colNumber; j += 3) {
            Y_sum_1 += data1[j];
            Y_sum_2 += data2[j];
        }
    }
    double alphaY = 0;
    if (Y_sum_1 > Y_sum_2) {
        alphaY = (double) Y_sum_1 / (double) Y_sum_2;
        *Y21Y12 = 1;
    } else {
        alphaY = (double) Y_sum_2 / (double) Y_sum_1;
        *Y21Y12 = 2;
    }
    *alpha21Y = alphaY;

}
/****************************************************************************************************************/

/*设置 YUV 颜色亮度均衡*********************************************************************************************/
void YUV_change(Mat src, Mat *dest, double alpha21Y, double alpha21U, double alpha21V) {
    //转换成 HSV 空间
    Mat image1_YUV;
    cvtColor(src, image1_YUV, COLOR_BGR2YUV);

    size_t rowNumber = image1_YUV.rows;
    size_t colNumber = image1_YUV.cols * image1_YUV.channels();

    Mat dst_YUV;
    dst_YUV = image1_YUV.clone();

    for (size_t i = 0; i < rowNumber; i++) {
        uchar *data1 = dst_YUV.ptr<uchar>(i);
        uchar *data2 = image1_YUV.ptr<uchar>(i);
        for (size_t j = 0; j < colNumber; j += 3) {
            int Y = cvRound((double) data2[j] * alpha21Y);
            data1[j] = (Y > 255) ? 255 : Y;
            int U = cvRound((double) data2[j + 1] * alpha21U);
            data1[j + 1] = (U > 255) ? 255 : U;
            int V = cvRound((double) data2[j + 2] * alpha21V);
            data1[j + 2] = (V > 255) ? 255 : V;
        }
    }
    Mat dest_BGR;
    cvtColor(dst_YUV, dest_BGR, COLOR_YUV2BGR);
#if IWSHOW_DEBUG
    imshow("src", src);
    imshow("dest_BGR", dest_BGR);
    waitKey();
#endif
    *dest = dest_BGR.clone();
}
/****************************************************************************************************************/

/*最佳缝合线******************************************************************************************************/
bool Optimal_Stitching_Line(Mat eq_img_0, Mat eq_img_1, int Stitching_threshold, Mat *m) {
/*4. 融合区最佳缝合线******************************************************************/
    /*a.计算缝合线判决值******************************/
    Mat diff, diff_Sobel_x, diff_Sobel_y;
    Mat eq_img_0_gray, eq_img_1_gray;
    cvtColor(eq_img_0, eq_img_0_gray, CV_BGR2GRAY);    //转换成灰度
    cvtColor(eq_img_1, eq_img_1_gray, CV_BGR2GRAY);

    eq_img_0_gray.convertTo(eq_img_0_gray, CV_32F, 1.0 / 255.0);    //转换成浮点
    eq_img_1_gray.convertTo(eq_img_1_gray, CV_32F, 1.0 / 255.0);

    absdiff(eq_img_0_gray, eq_img_1_gray, diff);        //图像差值

    Mat diff_2;
    diff_2 = diff.mul(diff);            //图像差值平方

    //改进 soble 算子
    Mat myKnernel_x = (Mat_<double>(3, 3) << -2, 0, 2, -1, 0, 1, -2, 0, 2);
    Mat myKnernel_y = (Mat_<double>(3, 3) << -2, -1, -2, 0, 0, 0, 2, 1, 2);

    Mat dst_x_0, dst_y_0, dst_x_1, dst_y_1;        //x，y 方向梯度值

    filter2D(eq_img_0_gray, dst_x_0, -1, myKnernel_x, Point(-1, -1), 0, BORDER_DEFAULT);
    filter2D(eq_img_0_gray, dst_y_0, -1, myKnernel_y, Point(-1, -1), 0, BORDER_DEFAULT);
    filter2D(eq_img_1_gray, dst_x_1, -1, myKnernel_x, Point(-1, -1), 0, BORDER_DEFAULT);
    filter2D(eq_img_1_gray, dst_y_1, -1, myKnernel_y, Point(-1, -1), 0, BORDER_DEFAULT);

    absdiff(dst_x_0, dst_x_1, diff_Sobel_x);        //x方向梯度差值
    absdiff(dst_y_0, dst_y_1, diff_Sobel_y);        //y方向梯度差值

    Mat diff_Sobel_xy = diff_Sobel_x.mul(diff_Sobel_y);        //(dx_0 - dx_1)*(dy_0 - dy_1)

    Mat geometry;
    geometry = diff_2 + diff_Sobel_xy;        //缝合线判决值 = 颜色(灰度)相似度 + 结构相似度

    /*b. 遍历判决值 Mat，查找缝合线********************************/
    int rowNumber = geometry.rows;
    int colNumber = geometry.cols * geometry.channels();
    vector<vector<Point> > all_line;
    vector<double> all_line_strength;
    vector<int> all_finish_loop;
    char finish_loop = 0;

    for (int i = 1; i < rowNumber - 1; i++)                //行循环
    {
        finish_loop = 0;
        int row_loop = i;
        vector<Point> line;
        double line_strength = 0;
        line_strength = (double) geometry.at<float>(row_loop, 0);
        for (int j = 1; j < colNumber - 1; j++)        //列循环
        {
            line.push_back(Point(j - 1, row_loop));
            float elm_middle = geometry.at<float>(row_loop, (j));
            float elm_left = geometry.at<float>((row_loop - 1), (j));
            float elm_right = geometry.at<float>((row_loop + 1), (j));
            int offse;
            if (elm_middle < elm_left) {
                if (elm_middle < elm_right) {
                    offse = 0;
                    line_strength += (double) elm_middle;
                }
                else {
                    offse = 1;
                    line_strength += (double) elm_right;
                }
            }
            else {
                if (elm_left < elm_right) {
                    offse = -1;
                    line_strength += (double) elm_left;
                }
                else {
                    offse = 1;
                    line_strength += (double) elm_right;
                }
            }
            row_loop = row_loop + offse;
            if (row_loop <= 1)//|| row_loop > rowNumber
            {
                row_loop = rowNumber / 2;
                finish_loop++;
            }
            else if (row_loop >= (rowNumber - 1)) //|| row_loop > rowNumber
            {
                finish_loop++;
                row_loop = rowNumber / 2;    //如果查找到边缘，返回图像中心重新寻找
            }
            if (finish_loop > Stitching_threshold) {
                break;
            }
        }
        if (finish_loop <= Stitching_threshold) {
            all_line.push_back(line);    //缝合线
            all_line_strength.push_back(line_strength);    //缝合线强度
            all_finish_loop.push_back(finish_loop);
        }
    }
    /*c. 在缝合线中找最小缝合线和********************************/
    int flm_min = 0;
    if (0 != all_line.size()) {
        cout << "找到缝合线条数 = " << all_line.size() << endl;
        for (size_t flm = 0; flm < all_line.size(); flm++) {
            if (all_line_strength[flm_min] >= all_line_strength[flm]) {
                flm_min = flm;
            }
        }
        if (0 != all_line.size()) {
            rowNumber = m->rows;
            colNumber = m->cols * m->channels();
            for (int rows = 0; rows < rowNumber; rows++) {
                float *data = m->ptr<float>(rows);
                for (int cols = 0; cols < colNumber; cols++) {
                    if (rows > all_line[flm_min][cols].y) {
                        data[cols] = 1;
                    }
                    else {
                        data[cols] = 0;
                    }
                }
            }
        }
#if IWSHOW_DEBUG
        //绘制所有缝合线
        Mat lines_mat_0 = eq_img_0.clone();
        Mat lines_mat_1 = eq_img_1.clone();
        uchar green_d = 255 / all_line.size();

        for(size_t flm = 0; flm < all_line.size(); flm++)
        {
            uchar green = green_d * flm;
            uchar blue = (255 - green);
            for(size_t lines = 0; lines < all_line[flm].size() - 1; lines++)
            {
                line(lines_mat_0 , all_line[flm][lines], all_line[flm][lines + 1], Scalar(blue, green, 0), 1, LINE_AA);
                line(lines_mat_1 , all_line[flm][lines], all_line[flm][lines + 1], Scalar(blue, green, 0), 1, LINE_AA);
            }
            std::cout << "all_line_strength = " << all_line_strength[flm] << endl;
            std::cout << "all_finish_loop = " << all_finish_loop[flm] << endl;
        }
        imshow("lines_mat_0", lines_mat_0);
        imshow("lines_mat_1", lines_mat_1);
        waitKey();
        //绘制最佳缝合线
        Mat lines_mat = eq_img_0.clone();
        for(size_t lines = 0; lines < all_line[flm_min].size() - 1; lines++)
        {
            line(lines_mat , all_line[flm_min][lines], all_line[flm_min][lines + 1], Scalar(255, 255, 0), 1, LINE_AA);
        }
        imshow("lines_mat", lines_mat);
        waitKey();
#endif
        return true;
    }
    else {
        cout << "未找到缝合线，线性融合 " << endl;
        return false;
    }
}
/****************************************************************************************************************/
