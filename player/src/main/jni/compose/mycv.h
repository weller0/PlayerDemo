/*
 * mycv.h
 *
 *  Created on: 2017-2-15
 *      Author: lyz1900
 */

#ifndef MYCV_H_
#define MYCV_H_

#include <opencv2/opencv.hpp>  //头文件
#include"opencv2/ml.hpp"

#define PI            ((float)3.1415926535897932384626433832795)
#define P2PI        ((float)6.283185307179586476925286766559)
#define PI2            ((float)1.5707963267948966192313216916398)
#define P_PI2        ((float)-1.5707963267948966192313216916398)
#define ANGLE    0.8    //((float)(180.0 / 220.0))		//((float)(180.0 / 220.0))

#define SURF_THRESHOLD 150

using namespace cv;  //包含cv命名空间
using namespace std;
using namespace cv::ml;

/* SRC0 生成完整映射情况 坐标变换*************************************************************************************/
void src_0_Equirectangular_coordinate_transformation(Point2f Texture_Coordinates,
                                                     Point2f *Image_Coordinates, Size src_size,
                                                     Point2f centerf_0, float radiusf_0,
                                                     Mat RotateMat, Vec3d TMatrix, double dbK);
/**************************************************************************************************************/
/* SRC1 生成完整映射情况 坐标变换*************************************************************************************/
void src_1_Equirectangular_coordinate_transformation(Point2f Texture_Coordinates,
                                                     Point2f *Image_Coordinates, Size src_size,
                                                     Point2f centerf_1, float radiusf_1);
/**************************************************************************************************************/

/* SRC0 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_0_map(Size src_size, Size dest_size,
                                        Point2f centerf_0, float radiusf_0,
                                        Mat RotateMat, Vec3d TMatrix, double dbK,
                                        Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC1 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_1_map(Size src_size, Size dest_size,
                                        Point2f centerf_1, float radiusf_1,
                                        Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC0 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_0_map_roi(Size src_size, Size dest_size,
                                            Point2f centerf_0, float radiusf_0,
                                            Mat RotateMat, Vec3d TMatrix, double dbK,
                                            Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC1 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_1_map_roi(Size src_size, Size dest_size,
                                            Point2f centerf_1, float radiusf_1,
                                            Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/

/* SRC0 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_0_map_2(Size src_size, Size dest_size,
                                          Point2f centerf_0, float radiusf_0,
                                          Mat RotateMat, Vec3d TMatrix, double dbK,
                                          Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC1 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_1_map_2(Size src_size, Size dest_size,
                                          Point2f centerf_1, float radiusf_1,
                                          Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC0 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_0_map_roi_2(Size src_size, Size dest_size,
                                              Point2f centerf_0, float radiusf_0,
                                              Mat RotateMat, Vec3d TMatrix, double dbK,
                                              Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/
/* SRC1 生成完整映射情况 map*************************************************************************************/
void Generate_Equirectangular_src_1_map_roi_2(Size src_size, Size dest_size,
                                              Point2f centerf_1, float radiusf_1,
                                              Mat *mapx_ud, Mat *mapy_ud);
/**************************************************************************************************************/

/*最佳缝合线****************************************************************************************************/
bool Optimal_Stitching_Line(Mat eq_img_0, Mat eq_img_1, int Stitching_threshold, Mat *m);
/**************************************************************************************************************/

/*寻找圆形区域中心半径*******************************************************************************************/
void Circumscribe(Mat src, int T, Point2f *centerf, float *radiusf, int num);
/**************************************************************************************************************/

/*亮度匹配到平均值***********************************************************************************************/
void YUV_match_avg(Mat src1, Mat src2, double *alpha1Y, double *alpha1U, double *alpha1V,
                   double *alpha2Y, double *alpha2U, double *alpha2V);
/**************************************************************************************************************/

/*获取亮度 K 值****************************************************************************************************/
void YUV_match(Mat src1, Mat src2, int *Y21Y12, double *alpha21Y);
/**************************************************************************************************************/

/*亮度设置到平均值***********************************************************************************************/
void YUV_change(Mat src, Mat *dest, double alpha21Y, double alpha21U, double alpha21V);
/**************************************************************************************************************/

#endif /* MYCV_H_ */
