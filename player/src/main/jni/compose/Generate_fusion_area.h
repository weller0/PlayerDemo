/*
 * Generate_fusion_area.h
 *
 *  Created on: 2017-4-19
 *      Author: lyz1900
 */

#ifndef GENERATE_FUSION_AREA_H_
#define GENERATE_FUSION_AREA_H_

#include <opencv2/opencv.hpp>  //头文件
#include"opencv2/ml.hpp"

using namespace cv;  //包含cv命名空间
using namespace std;
using namespace cv::ml;

void Generate_fusion_area_init(
        Size src_size,                                                                        //输入原图大小
        Point2f center_all_1, Point2f center_all_2,                    //两个镜头中心参数
        float rad_all_1, float rad_all_2,                                        //镜头有效区域半径参数
        Mat RotationMatrix, Vec3d TMatrix, double pdK,        //3D 点标定外参
        Mat *mapx_roi0, Mat *mapy_roi0,                                            //imag_0 经纬展开 map
        Mat *mapx_roi1, Mat *mapy_roi1,                                            //imag_1 经纬展开 map
        Mat *m);                                                    //融合区 Mark

void Generate_fusion_area(
        Size src_size,                                                                        //输入原图大小
        Mat img_0, //Mat img_1,				//输入原图片
        Mat mapx_roi0, Mat mapy_roi0,                                            //imag_0 经纬展开 map
        Mat mapx_roi1, Mat mapy_roi1,                                            //imag_1 经纬展开 map
        Mat m,                                                                                //融合区 Mark
        Mat *img_out);                                                        //输出融合区图像

#endif /* GENERATE_FUSION_AREA_H_ */
