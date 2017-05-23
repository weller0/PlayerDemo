/*
 * Generate_fusion_area.cpp
 *
 *  Created on: 2017-4-19
 *      Author: lyz1900
 */

#include"mycv.h"
#include"LaplacianBlending.h"
#include"Generate_fusion_area.h"

#define SRC_IMAG_SIZE Size(3264, 2448)            //输入原图大小
#define DEST_IMAG_SIZE_WIDTH  2000                //输出球面等距投影宽
#define DEST_IMAG_SIZE_HIGHT  (DEST_IMAG_SIZE_WIDTH/2)                //输出球面等距投影宽ANGLE
#define EQ_IMAG_SIZE Size(DEST_IMAG_SIZE_WIDTH, cvRound((float)(DEST_IMAG_SIZE_HIGHT+2)/(1.0+ANGLE)))   //生成 ROI 区域和非重叠区域的大小
#define FUSE_W 16

void Generate_fusion_area_init_YUV420SP(
        Size src_size,                                                                        //输入原图大小
        Point2f center_all_1, Point2f center_all_2,                    //两个镜头中心参数
        float rad_all_1, float rad_all_2,                                        //镜头有效区域半径参数
        Mat RotationMatrix, Vec3d TMatrix, double pdK,        //3D 点标定外参
        Mat *mapx_roi0, Mat *mapy_roi0,                                            //imag_0 经纬展开 map
        Mat *mapx_roi1, Mat *mapy_roi1,                                            //imag_1 经纬展开 map
        Mat *mapx_roi0_2,
        Mat *mapy_roi0_2,                                            //imag_0 经纬展开 map
        Mat *mapx_roi1_2,
        Mat *mapy_roi1_2,                                            //imag_1 经纬展开 map
        Mat *m,
        Mat *m_uv)                                                        //融合区 Mark
{
    float zoom_alpha;
    // float rad_max = (rad_all_1>rad_all_2)?rad_all_1*2:rad_all_2*2;
    // zoom_alpha = (src_size.height/rad_max);
    zoom_alpha = 1.0;
    //imag_0 经纬展开 ROI 融合区 map
    Generate_Equirectangular_src_0_map_roi_yuv420sp(src_size, EQ_IMAG_SIZE, center_all_1, rad_all_1,
                                                    RotationMatrix, TMatrix, pdK,
                                                    mapx_roi0, mapy_roi0,
                                                    mapx_roi0_2, mapy_roi0_2, zoom_alpha);
    //imag_1 经纬展开 ROI 融合区 map
    Generate_Equirectangular_src_1_map_roi_yuv420sp(src_size, EQ_IMAG_SIZE, center_all_2, rad_all_2,
                                                    mapx_roi1, mapy_roi1,
                                                    mapx_roi1_2, mapy_roi1_2, zoom_alpha);

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
    *m = m_line.clone();
    *m_uv = im_uv.clone();
}

void Generate_fusion_area_YUV420P(
        Size src_size,                                                                        //输入原图大小
        Mat img_Y,                                            //Mat img_1,				//输入原图片
        Mat img_U,                                            //Mat img_1,				//输入原图片
        Mat img_V,                                            //Mat img_1,				//输入原图片
        Mat mapx_roi0, Mat mapy_roi0,                                            //imag_0 经纬展开 map
        Mat mapx_roi1, Mat mapy_roi1,                                            //imag_1 经纬展开 map
        Mat mapx_roi0_2,
        Mat mapy_roi0_2,                                            //imag_0 经纬展开 map
        Mat mapx_roi1_2,
        Mat mapy_roi1_2,                                            //imag_1 经纬展开 map
        Mat m,                                                                                //融合区 Mark
        Mat m_uv,                                                                                //融合区 Mark
        Mat *img_out_Y,                                                                    //输出融合区图像
        Mat *img_out_U,                                                                    //输出融合区图像
        Mat *img_out_V                                                                    //输出融合区图像
) {
    /*A. 展开拼接部分图像************************************************************/
    Mat eq_imgY_0, eq_imgY_1;
    Mat eq_imgU_0, eq_imgU_1;
    Mat eq_imgV_0, eq_imgV_1;

    remap(img_Y, eq_imgY_0, mapx_roi0, mapy_roi0, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_Y, eq_imgY_1, mapx_roi1, mapy_roi1, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_U, eq_imgU_0, mapx_roi0_2, mapy_roi0_2, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_U, eq_imgU_1, mapx_roi1_2, mapy_roi1_2, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_V, eq_imgV_0, mapx_roi0_2, mapy_roi0_2, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_V, eq_imgV_1, mapx_roi1_2, mapy_roi1_2, INTER_LINEAR, BORDER_CONSTANT);

    /*B. 统计拼接部分图像亮度************************************************************/
    double alpha1Y;            //融合区得到的亮度比例值
    double alpha2Y;            //融合区得到的亮度比例值

    /*统计 yuv420p 图像数据 Y 通道图像亮度差*/
    YUV_match_avg_yuv420p(eq_imgY_0, eq_imgY_1, &alpha1Y, &alpha2Y);

    /*分割融合区，降低融合区域计算量*/
    int start_roi0 = eq_imgY_0.rows / 2 - FUSE_W + eq_imgY_0.rows % 2;
    int start_roi1 = start_roi0 + m.rows;
    int roi1_hight = eq_imgY_1.rows - start_roi1;

    Rect2i little_rec = Rect(0, start_roi0 + 1, eq_imgY_0.cols, m.rows);        //取到重叠区
    Rect2i rec_0 = Rect(0, 0, eq_imgY_0.cols, start_roi0);        //取到非重叠区
    Rect2i rec_1 = Rect(0, start_roi1, eq_imgY_1.cols, roi1_hight);        //取到非重叠区

    Rect2i little_rec_uv = Rect(0, (start_roi0 + 1) / 2, eq_imgY_0.cols / 2,
                                m.rows / 2);        //取到重叠区
    Rect2i rec_uv_0 = Rect(0, 0, eq_imgY_0.cols / 2, start_roi0 / 2);        //取到非重叠区
    Rect2i rec_uv_1 = Rect(0, start_roi1 / 2, eq_imgY_1.cols / 2, roi1_hight / 2);        //取到非重叠区

    Mat little_roiY_0 = eq_imgY_0(little_rec);
    Mat little_roiY_1 = eq_imgY_1(little_rec);

    Mat little_roiU_0 = eq_imgU_0(little_rec_uv);
    Mat little_roiU_1 = eq_imgU_1(little_rec_uv);

    Mat little_roiV_0 = eq_imgV_0(little_rec_uv);
    Mat little_roiV_1 = eq_imgV_1(little_rec_uv);

    /*C. 拉普拉斯融合*/
    Mat_<float> L_Y, L_U, L_V;
    Mat_<float> R_Y, R_U, R_V;

    little_roiY_0.convertTo(R_Y, CV_32F, 1.0 / 255.0);
    little_roiY_1.convertTo(L_Y, CV_32F, 1.0 / 255.0);

    little_roiU_0.convertTo(R_U, CV_32F, 1.0 / 255.0);
    little_roiU_1.convertTo(L_U, CV_32F, 1.0 / 255.0);

    little_roiV_0.convertTo(R_V, CV_32F, 1.0 / 255.0);
    little_roiV_1.convertTo(L_V, CV_32F, 1.0 / 255.0);

    Mat_<float> blend_Y = LaplacianBlend_YUV420P(L_Y, R_Y, m);            //拉普拉斯融合	Y
    Mat_<float> blend_U = LaplacianBlend_YUV420P(L_U, R_U, m_uv);        //拉普拉斯融合	U
    Mat_<float> blend_V = LaplacianBlend_YUV420P(L_V, R_V, m_uv);            //拉普拉斯融合	V


    Mat_<uchar> out_Y;        //融合结果（转换为 U8）
    Mat_<uchar> out_U;        //融合结果（转换为 U8）
    Mat_<uchar> out_V;        //融合结果（转换为 U8）

    blend_Y.convertTo(out_Y, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    blend_U.convertTo(out_U, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    blend_V.convertTo(out_V, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型

    //取到非重叠区
    Mat out_roiY_0 = eq_imgY_0(rec_0);
    Mat out_roiY_1 = eq_imgY_1(rec_1);
    Mat out_roiU_0 = eq_imgU_0(rec_uv_0);
    Mat out_roiU_1 = eq_imgU_1(rec_uv_1);
    Mat out_roiV_0 = eq_imgV_0(rec_uv_0);
    Mat out_roiV_1 = eq_imgV_1(rec_uv_1);

    Mat Splice_image_Y;
    Mat Splice_image_U;
    Mat Splice_image_V;

    //Y
    vector<Mat> out_image_vectorY;
    out_image_vectorY.push_back(out_roiY_0);
    out_image_vectorY.push_back(out_Y);
    out_image_vectorY.push_back(out_roiY_1);
    vconcat(out_image_vectorY, Splice_image_Y);
    //U
    vector<Mat> out_image_vectorU;
    out_image_vectorU.push_back(out_roiU_0);
    out_image_vectorU.push_back(out_U);
    out_image_vectorU.push_back(out_roiU_1);
    vconcat(out_image_vectorU, Splice_image_U);
    //V
    vector<Mat> out_image_vectorV;
    out_image_vectorV.push_back(out_roiV_0);
    out_image_vectorV.push_back(out_V);
    out_image_vectorV.push_back(out_roiV_1);
    vconcat(out_image_vectorV, Splice_image_V);

    *img_out_Y = Splice_image_Y.clone();
    *img_out_U = Splice_image_U.clone();
    *img_out_V = Splice_image_V.clone();
}