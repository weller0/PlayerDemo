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

/*统计 yuv420p 图像数据 Y 通道图像亮度差*/
void YUV_match_avg_yuv420p(Mat src1, Mat src2,
                           double *alpha1Y, double *alpha2Y);

void Generate_Equirectangular_src_0_map_roi_yuv420sp(Size src_size, Size dest_size,
                                                     Point2f centerf_0, float radiusf_0,
                                                     Mat RotateMat, Vec3d TMatrix, double dbK,
                                                     Mat *mapx_ud, Mat *mapy_ud,
                                                     Mat *mapx_ud_2, Mat *mapy_ud_2);//,float zoom);

void Generate_Equirectangular_src_1_map_roi_yuv420sp(Size src_size, Size dest_size,
                                                     Point2f centerf_1, float radiusf_1,
                                                     Mat *mapx_ud, Mat *mapy_ud,
                                                     Mat *mapx_ud_2, Mat *mapy_ud_2);//, float zoom);
/*最佳缝合线****************************************************************************************************/
bool Optimal_Stitching_Line(Mat eq_img_0, Mat eq_img_1, int Stitching_threshold, Mat *m);
/**************************************************************************************************************/

/*寻找圆形区域中心半径*******************************************************************************************/
void Circumscribe(Mat src, int T, Point2f *centerf, float *radiusf, int num);
/**************************************************************************************************************/
#endif /* MYCV_H_ */
