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
#define DEST_IMAG_SIZE_WIDTH  1920                //输出球面等距投影宽
#define DEST_IMAG_SIZE_HIGHT  (DEST_IMAG_SIZE_WIDTH/2)                //输出球面等距投影宽ANGLE
#define EQ_IMAG_SIZE Size(DEST_IMAG_SIZE_WIDTH, cvRound((float)(DEST_IMAG_SIZE_HIGHT+2)/(1.0+ANGLE)))   //生成 ROI 区域和非重叠区域的大小
#define FUSE_W 8

void Generate_fusion_area_init(
        Size src_size,                                                                        //输入原图大小
        Point2f center_all_1, Point2f center_all_2,                    //两个镜头中心参数
        float rad_all_1, float rad_all_2,                                        //镜头有效区域半径参数
        Mat RotationMatrix, Vec3d TMatrix, double pdK,        //3D 点标定外参
        Mat *mapx_roi0, Mat *mapy_roi0,                                            //imag_0 经纬展开 map
        Mat *mapx_roi1, Mat *mapy_roi1,                                            //imag_1 经纬展开 map
        Mat *m)                                                        //融合区 Mark
{
    //imag_0 经纬展开 ROI 融合区 map
    Generate_Equirectangular_src_0_map_roi(src_size, EQ_IMAG_SIZE, center_all_1, rad_all_1,
                                           RotationMatrix, TMatrix, pdK, mapx_roi0, mapy_roi0);
    //imag_1 经纬展开 ROI 融合区 map
    Generate_Equirectangular_src_1_map_roi(src_size, EQ_IMAG_SIZE, center_all_2, rad_all_2,
                                           mapx_roi1, mapy_roi1);

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

    *m = m_line.clone();
}

void Generate_fusion_area(
        Size src_size,                                                                        //输入原图大小
        Mat img_0,                                            //Mat img_1,				//输入原图片
        Mat mapx_roi0, Mat mapy_roi0,                                            //imag_0 经纬展开 map
        Mat mapx_roi1, Mat mapy_roi1,                                            //imag_1 经纬展开 map
        Mat m,                                                                                //融合区 Mark
        Mat *img_out                                                                    //输出融合区图像
) {
    Mat eq_img_0, eq_img_1;
    remap(img_0, eq_img_0, mapx_roi0, mapy_roi0, INTER_LINEAR, BORDER_CONSTANT);
    remap(img_0, eq_img_1, mapx_roi1, mapy_roi1, INTER_LINEAR, BORDER_CONSTANT);

    double alpha1Y;            //融合区得到的亮度比例值
    double alpha1U;
    double alpha1V;

    double alpha2Y;            //融合区得到的亮度比例值
    double alpha2U;
    double alpha2V;

    YUV_match_avg(eq_img_0, eq_img_1, &alpha1Y, &alpha1U, &alpha1V, &alpha2Y, &alpha2U, &alpha2V);


    int start_roi0 = eq_img_0.rows / 2 - FUSE_W + eq_img_0.rows % 2;
    int start_roi1 = start_roi0 + m.rows;
    int roi1_hight = eq_img_1.rows - start_roi1;

    Rect2i little_rec = Rect(0, start_roi0 + 1, eq_img_0.cols, m.rows);        //取到重叠区

    Rect2i rec_0 = Rect(0, 0, eq_img_0.cols, start_roi0);        //取到非重叠区
    Rect2i rec_1 = Rect(0, start_roi1, eq_img_1.cols, roi1_hight);        //取到非重叠区

    Mat little_roi_0 = eq_img_0(little_rec);
    Mat little_roi_1 = eq_img_1(little_rec);

    Mat_<Vec3f> l;
    Mat_<Vec3f> r;
    little_roi_0.convertTo(r, CV_32F, 1.0 / 255.0);        //Vec3f表示有三个通道，即 l[row][column][depth]
    little_roi_1.convertTo(l, CV_32F, 1.0 / 255.0);

    Mat_<Vec3f> blend = LaplacianBlend(l, r, m);        //拉普拉斯融合
    Mat_<Vec3b> out;        //融合结果（转换为 U8）
    blend.convertTo(out, CV_8U, 255.0, 0.0);        //转换为 U8C3 数据类型
    Mat out_roi_0 = eq_img_0(rec_0);
    Mat out_roi_1 = eq_img_1(rec_1);
    vector<Mat> out_image_vector;
    out_image_vector.push_back(out_roi_0);
    out_image_vector.push_back(out);
    out_image_vector.push_back(out_roi_1);
    Mat Splice_image;
    vconcat(out_image_vector, Splice_image);
    *img_out = Splice_image.clone();
}