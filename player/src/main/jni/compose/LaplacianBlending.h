/*
 * LaplacianBlending.h
 *
 *  Created on: 2017-3-29
 *      Author: lyz1900
 */

#ifndef LAPLACIANBLENDING_H_
#define LAPLACIANBLENDING_H_

#include <opencv2/opencv.hpp>  //头文件
#include"opencv2/ml.hpp"

using namespace cv;  //包含cv命名空间
using namespace std;
using namespace cv::ml;


Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f> &l, const Mat_<Vec3f> &r, const Mat_<float> &m);

#endif /* LAPLACIANBLENDING_H_ */
