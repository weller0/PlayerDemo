/*
 * Generate_fusion_area.cpp
 *
 *  Created on: 2017-4-19
 *      Author: lyz1900
 */

#include"mycv.h"
#include"LaplacianBlending.h"
#include"Generate_fusion_area.h"
#include "log.h"

#define SRC_IMAG_SIZE Size(3264, 2448)            //输入原图大小
#define DEST_IMAG_SIZE_WIDTH  2000                //输出球面等距投影宽
#define DEST_IMAG_SIZE_HIGHT  (DEST_IMAG_SIZE_WIDTH/2)                //输出球面等距投影宽ANGLE
#define EQ_IMAG_SIZE Size(DEST_IMAG_SIZE_WIDTH, cvRound((float)(DEST_IMAG_SIZE_HIGHT+2)/(1.0+ANGLE)))   //生成 ROI 区域和非重叠区域的大小
#define FUSE_W 16

#define OPENCV_DEBUG 0    //是否打印该文件内打印信息

void Generate_fusion_area_init_YUV420SP(
        Size src_size,                                    //输入原图大小
        Point2f center_all_1, Point2f center_all_2,        //两个镜头中心参数
        float rad_all_1, float rad_all_2,                //镜头有效区域半径参数
        Mat RotationMatrix, Vec3d TMatrix, double pdK,    //3D 点标定外参
        Mat *mapx_roi0, Mat *mapy_roi0,                    //imag_0 经纬展开 map Y
        Mat *mapx_roi1, Mat *mapy_roi1,                    //imag_1 经纬展开 map Y
        Mat *mapx_roi0_2, Mat *mapy_roi0_2,              //imag_0 经纬展开 map U V
        Mat *mapx_roi1_2, Mat *mapy_roi1_2,              //imag_1 经纬展开 map U V
        Mat *m, Mat *m_uv)                                //融合区 Mark Y, Mark UV
{
    //float zoom_alpha;
    // float rad_max = (rad_all_1>rad_all_2)?rad_all_1*2:rad_all_2*2;
    // zoom_alpha = (src_size.height/rad_max);
    //zoom_alpha = 1.0;
    //imag_0 经纬展开 ROI 融合区 map
    Point2f center1 = Point2f(rad_all_1, rad_all_1);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP rad_all_1 %f", rad_all_1);
    Generate_Equirectangular_src_0_map_roi_yuv420sp(src_size, EQ_IMAG_SIZE, center1,
                                                    rad_all_1,//center_all_1
                                                    RotationMatrix, TMatrix, pdK,
                                                    mapx_roi0, mapy_roi0,
                                                    mapx_roi0_2, mapy_roi0_2);//, zoom_alpha);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP mapx_roi0 cols %d,rows %d, mapx_roi0_uv cols %d,rows %d",
                                        mapx_roi0->cols, mapx_roi0->rows,mapx_roi0_2->cols, mapx_roi0_2->rows);
#endif
    //imag_1 经纬展开 ROI 融合区 map
    int w2r1 = cvRound(rad_all_1 * 2);
    if (w2r1 % 4) {
        w2r1 = w2r1 + (4 - w2r1 % 4);
    }
    float center_x = (float) w2r1 + rad_all_2;
    Point2f center2 = Point2f(center_x, rad_all_2);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP src_size.width %d", src_size.width);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP src_size.height %d", src_size.height);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP rad_all_2 %f", rad_all_2);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP center2_x %f", center_x);
    Generate_Equirectangular_src_1_map_roi_yuv420sp(src_size, EQ_IMAG_SIZE, center2,
                                                    rad_all_2,//center_all_2
                                                    mapx_roi1, mapy_roi1,
                                                    mapx_roi1_2, mapy_roi1_2);//, zoom_alpha);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP mapx_roi1 cols %d,rows %d, mapx_roi1_uv cols %d,rows %d",
    mapx_roi1->cols, mapx_roi1->rows,mapx_roi1_2->cols, mapx_roi1_2->rows);
#endif

    /*生成拼接区线性融合 map**********************/
    int width = EQ_IMAG_SIZE.width;
    int roi_hight = FUSE_W * 2;                                        //roi 区域的高度
    Mat_<float> m_line(roi_hight, width, 1.0);           //定义融合区域 Mask，初始值为 1
    int fuse_area = FUSE_W;                                            //定义融合区交接，线性融合宽度
    float color_v[FUSE_W * 2] = {0};
    for (int a = fuse_area / 2; a < roi_hight - fuse_area / 2; a++)        //Mask 上标注线性融合区域
    {
        //m_line(Range(a,a + 1), Range::all()) = (float(a) * (0.5 / (float)fuse_area)); //m.cols/2取m全部行&[0,m.cols/2]列，赋值为1.0
        float color = (float(a - fuse_area / 2 + 1) * (1 / (float) (roi_hight / 2)));
        //line(m_line, Point(0, a), Point(width, a),color,1, LINE_8);
        m_line(Range(a, a + 1), Range::all()) = color;

    }
    m_line(Range(0, fuse_area / 2), Range::all()) = 0.0;
    m_line(Range(roi_hight - fuse_area / 2, roi_hight), Range::all()) = 1.0;
    for (int i = 1; i < roi_hight; i++)        //Mask 上标注线性融合区域
    {
        color_v[i] = m_line.at<float>(i, 0);
    }
    Mat im_uv;
    resize(m_line, im_uv, Size(m_line.cols / 2, m_line.rows / 2), INTER_LINEAR);

#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP m_line cols %d,rows %d",m_line.cols, m_line.rows);
    LOGD("[PlayYuv] Generate_fusion_area_init_YUV420SP im_uv cols %d,rows %d",im_uv.cols, im_uv.rows);
    imwrite("/storage/emulated/0/Movies/m_line.jpg", m_line );
    imwrite("/storage/emulated/0/Movies/im_uv.jpg", im_uv );
#endif
    *m = m_line.clone();
    *m_uv = im_uv.clone();
}

void Generate_fusion_area_YUV420P(
        Size src_size,                                    //输入原图大小
        Mat img_Y, Mat img_U, Mat img_V,                  //输入原图片 Y U V 通道
        Mat mapx_roi0, Mat mapy_roi0,                   //imag_0 经纬展开 map
        Mat mapx_roi1, Mat mapy_roi1,                   //imag_1 经纬展开 map
        Mat mapx_roi0_2, Mat mapy_roi0_2,                //imag_0 经纬展开 map
        Mat mapx_roi1_2, Mat mapy_roi1_2,                //imag_1 经纬展开 map
        Mat m, Mat m_uv,                                 //融合区 Mark
        double *alpha1Y, double *alpha2Y,                    //输出两个半球亮度调整的值
        Mat *img_out_Y, Mat *img_out_U, Mat *img_out_V)    //输出融合区图像
{
    /*A. 展开拼接部分图像************************************************************/
    Mat eq_imgY_0, eq_imgY_1;
    Mat eq_imgU_0, eq_imgU_1;
    Mat eq_imgV_0, eq_imgV_1;
    double time0 = static_cast<double>(getTickCount());
    remap(img_Y, eq_imgY_1, mapx_roi0, mapy_roi0, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time y0 %f", time0);
    time0 = static_cast<double>(getTickCount());
    remap(img_Y, eq_imgY_0, mapx_roi1, mapy_roi1, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time y1 %f", time0);
    time0 = static_cast<double>(getTickCount());
    remap(img_U, eq_imgU_1, mapx_roi0_2, mapy_roi0_2, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time u0 %f", time0);
    time0 = static_cast<double>(getTickCount());
    remap(img_U, eq_imgU_0, mapx_roi1_2, mapy_roi1_2, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time u1 %f", time0);
    time0 = static_cast<double>(getTickCount());
    remap(img_V, eq_imgV_1, mapx_roi0_2, mapy_roi0_2, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time v0 %f", time0);
    time0 = static_cast<double>(getTickCount());
    remap(img_V, eq_imgV_0, mapx_roi1_2, mapy_roi1_2, INTER_LINEAR, BORDER_CONSTANT);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P remap run time v1 %f", time0);
#if OPENCV_DEBUG
    imwrite("/storage/emulated/0/Movies/img_Y.jpg", img_Y );
    imwrite("/storage/emulated/0/Movies/img_U.jpg", img_U );
    imwrite("/storage/emulated/0/Movies/img_V.jpg", img_V );

    imwrite("/storage/emulated/0/Movies/eq_imgY_0.jpg", eq_imgY_0 );
    imwrite("/storage/emulated/0/Movies/eq_imgY_1.jpg", eq_imgY_1 );
    imwrite("/storage/emulated/0/Movies/eq_imgU_0.jpg", eq_imgU_0 );
    imwrite("/storage/emulated/0/Movies/eq_imgU_1.jpg", eq_imgU_1 );
    imwrite("/storage/emulated/0/Movies/eq_imgV_0.jpg", eq_imgV_0 );
    imwrite("/storage/emulated/0/Movies/eq_imgV_1.jpg", eq_imgV_1 );

    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgY_0 cols %d,rows %d",eq_imgY_0.cols, eq_imgY_0.rows);
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgY_1 cols %d,rows %d",eq_imgY_1.cols, eq_imgY_1.rows);
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgU_0 cols %d,rows %d",eq_imgU_0.cols, eq_imgU_0.rows);
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgU_1 cols %d,rows %d",eq_imgU_1.cols, eq_imgU_1.rows);
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgV_0 cols %d,rows %d",eq_imgV_0.cols, eq_imgV_0.rows);
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P eq_imgV_1 cols %d,rows %d",eq_imgV_1.cols, eq_imgV_1.rows);
#endif
    /*B. 统计拼接部分图像亮度************************************************************/
    /*统计 yuv420p 图像数据 Y 通道图像亮度差*/
    time0 = static_cast<double>(getTickCount());
    YUV_match_avg_yuv420p(eq_imgY_0, eq_imgY_1, alpha1Y, alpha2Y);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P YUV_match_avg_yuv420p time %f", time0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] YUV_match_avg_yuv420p alpha1Y %lf,alpha2Y %lf",*alpha1Y, *alpha2Y);
#endif
    time0 = static_cast<double>(getTickCount());
    /*分割融合区，降低融合区域计算量*/
    int start_roi0 = eq_imgY_0.rows / 2 - FUSE_W + eq_imgY_0.rows % 2;
    int start_roi1 = start_roi0 + m.rows;
    int roi1_hight = eq_imgY_1.rows - start_roi1;

    Rect2i little_rec = Rect(0, start_roi0 + 1, eq_imgY_0.cols, m.rows);        //取到重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P little_rec point(%d,%d) size(%d,%d)",little_rec.x,little_rec.y,little_rec.width,little_rec.height);
#endif
    Rect2i rec_0 = Rect(0, 0, eq_imgY_0.cols, start_roi0);        //取到非重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P rec_0 point(%d,%d) size(%d,%d)",rec_0.x,rec_0.y,rec_0.width,rec_0.height);
#endif
    Rect2i rec_1 = Rect(0, start_roi1, eq_imgY_1.cols, roi1_hight);        //取到非重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P rec_1 point(%d,%d) size(%d,%d)",rec_1.x,rec_1.y,rec_1.width,rec_1.height);
#endif

    Rect2i little_rec_uv = Rect(0, (start_roi0 + 1) / 2, eq_imgY_0.cols / 2,
                                m.rows / 2);        //取到重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P little_rec_uv point(%d,%d) size(%d,%d)",little_rec_uv.x,little_rec_uv.y,little_rec_uv.width,little_rec_uv.height);
#endif
    Rect2i rec_uv_0 = Rect(0, 0, eq_imgY_0.cols / 2, start_roi0 / 2);        //取到非重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P rec_uv_0 point(%d,%d) size(%d,%d)",rec_uv_0.x,rec_uv_0.y,rec_uv_0.width,rec_uv_0.height);
#endif
    Rect2i rec_uv_1 = Rect(0, start_roi1 / 2, eq_imgY_1.cols / 2, roi1_hight / 2);        //取到非重叠区
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P rec_uv_1 point(%d,%d) size(%d,%d)",rec_uv_1.x,rec_uv_1.y,rec_uv_1.width,rec_uv_1.height);
#endif
    Mat little_roiY_0 = eq_imgY_0(little_rec);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiY_0 size(%d,%d)",little_roiY_0.cols,little_roiY_0.rows);
#endif
    Mat little_roiY_1 = eq_imgY_1(little_rec);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiY_1 size(%d,%d)",little_roiY_1.cols,little_roiY_1.rows);
#endif

    Mat little_roiU_0 = eq_imgU_0(little_rec_uv);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiU_0 size(%d,%d)",little_roiU_0.cols,little_roiU_0.rows);
#endif
    Mat little_roiU_1 = eq_imgU_1(little_rec_uv);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiU_1 size(%d,%d)",little_roiU_1.cols,little_roiU_1.rows);
#endif
    Mat little_roiV_0 = eq_imgV_0(little_rec_uv);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiV_0 size(%d,%d)",little_roiV_0.cols,little_roiV_0.rows);
#endif
    Mat little_roiV_1 = eq_imgV_1(little_rec_uv);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P start get ROI little_roiV_1 size(%d,%d)",little_roiV_1.cols,little_roiV_1.rows);
#endif
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P get rect time %f", time0);
    time0 = static_cast<double>(getTickCount());
    /*C. 拉普拉斯融合*/
    Mat_<float> L_Y, L_U, L_V;
    Mat_<float> R_Y, R_U, R_V;
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start convert to float");
#endif
    if (*alpha1Y > (1.0 - 1e-8)) {
        little_roiY_0.convertTo(R_Y, CV_32F, (1.0 / 255.0));
        little_roiY_1.convertTo(L_Y, CV_32F, (*alpha2Y / 255.0));
    } else {
        little_roiY_0.convertTo(R_Y, CV_32F, (*alpha1Y / 255.0));
        little_roiY_1.convertTo(L_Y, CV_32F, (1.0 / 255.0));
    }


    little_roiU_0.convertTo(R_U, CV_32F, 1.0 / 255.0);
    little_roiU_1.convertTo(L_U, CV_32F, 1.0 / 255.0);

    little_roiV_0.convertTo(R_V, CV_32F, 1.0 / 255.0);
    little_roiV_1.convertTo(L_V, CV_32F, 1.0 / 255.0);
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P u8 to float time %f", time0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P convert to float finish");
#endif
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start LaplacianBlend");
#endif
    time0 = static_cast<double>(getTickCount());
//    Mat_<float> blend_Y = LaplacianBlend_YUV420P(L_Y, R_Y, m);            //拉普拉斯融合	Y
//    Mat_<float> blend_U = LaplacianBlend_YUV420P(L_U, R_U, m_uv);        //拉普拉斯融合	U
//    Mat_<float> blend_V = LaplacianBlend_YUV420P(L_V, R_V, m_uv);            //拉普拉斯融合	V
    Mat_<float> blend_Y = LaplacianBlend_YUV420P(R_Y, L_Y, m);            //拉普拉斯融合	Y
    Mat_<float> blend_U = LaplacianBlend_YUV420P(R_U, L_U, m_uv);        //拉普拉斯融合	U
    Mat_<float> blend_V = LaplacianBlend_YUV420P(R_V, L_V, m_uv);            //拉普拉斯融合	V
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P LaplacianBlend_YUV420P time %f", time0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P LaplacianBlend end");
#endif
    time0 = static_cast<double>(getTickCount());
    Mat_<uchar> out_Y;        //融合结果（转换为 U8）
    Mat_<uchar> out_U;        //融合结果（转换为 U8）
    Mat_<uchar> out_V;        //融合结果（转换为 U8）
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P convert result to u8");
#endif
    blend_Y.convertTo(out_Y, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    blend_U.convertTo(out_U, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    blend_V.convertTo(out_V, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P float to u8 time %f", time0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P convert result to u8 end");
#endif
    time0 = static_cast<double>(getTickCount());
    //取到非重叠区
    Mat out_roiY_0 = eq_imgY_1(rec_0);//rec_0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiY_0 size(%d,%d)",out_roiY_0.cols,out_roiY_0.rows);
#endif
    Mat out_roiY_1 = eq_imgY_0(rec_1);//rec_1);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiY_1 size(%d,%d)",out_roiY_1.cols,out_roiY_1.rows);
#endif
    Mat out_roiU_0 = eq_imgU_1(rec_uv_0);//rec_uv_0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiU_0 size(%d,%d)",out_roiU_0.cols,out_roiU_0.rows);
#endif
    Mat out_roiU_1 = eq_imgU_0(rec_uv_1);//rec_uv_1);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiU_1 size(%d,%d)",out_roiU_1.cols,out_roiU_1.rows);
#endif
    Mat out_roiV_0 = eq_imgV_1(rec_uv_0);//rec_uv_0);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiV_0 size(%d,%d)",out_roiV_0.cols,out_roiV_0.rows);
#endif
    Mat out_roiV_1 = eq_imgV_0(rec_uv_1);//rec_uv_1);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P start get Non-Overlapping ROI out_roiV_1 size(%d,%d)",out_roiV_1.cols,out_roiV_1.rows);
#endif

    Mat Splice_image_Y;
    Mat Splice_image_U;
    Mat Splice_image_V;

    //Y
    vector<Mat> out_image_vectorY;
    out_image_vectorY.push_back(out_roiY_0);
    out_image_vectorY.push_back(out_Y);
    out_image_vectorY.push_back(out_roiY_1);
    vconcat(out_image_vectorY, Splice_image_Y);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P RESULT Splice_image_Y size(%d,%d)",Splice_image_Y.cols,Splice_image_Y.rows);
#endif
    //U
    vector<Mat> out_image_vectorU;
    out_image_vectorU.push_back(out_roiU_0);
    out_image_vectorU.push_back(out_U);
    out_image_vectorU.push_back(out_roiU_1);
    vconcat(out_image_vectorU, Splice_image_U);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P RESULT Splice_image_U size(%d,%d)",Splice_image_U.cols,Splice_image_U.rows);
#endif
    //V
    vector<Mat> out_image_vectorV;
    out_image_vectorV.push_back(out_roiV_0);
    out_image_vectorV.push_back(out_V);
    out_image_vectorV.push_back(out_roiV_1);
    vconcat(out_image_vectorV, Splice_image_V);
#if OPENCV_DEBUG
    LOGD("[PlayYuv] LaplacianBlend_YUV420P RESULT Splice_image_V size(%d,%d)",Splice_image_V.cols,Splice_image_V.rows);
#endif
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P image bind time %f", time0);
    time0 = static_cast<double>(getTickCount());
    *img_out_Y = Splice_image_Y.clone();
    *img_out_U = Splice_image_U.clone();
    *img_out_V = Splice_image_V.clone();
    time0 = ((double) getTickCount() - time0) / getTickFrequency();
    LOGD("[PlayYuv] Generate_fusion_area_YUV420P result clone time %f", time0);
}