/*
 * LaplacianBlending.cpp
 *
 *  Created on: 2017-3-29
 *      Author: lyz1900
 */

#include"LaplacianBlending.h"

class LaplacianBlending {
private:
    Mat_<Vec3f> left;
    Mat_<Vec3f> right;
    Mat_<float> blendMask;

    vector<Mat_<Vec3f> > leftLapPyr, rightLapPyr, resultLapPyr;
    //Laplacian Pyramids
    Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
    vector<Mat_<Vec3f> > maskGaussianPyramid; //masks are 3-channels for easier multiplication with RGB

    size_t levels;

    void buildPyramids() {
        //创建左拉布拉斯金字塔
        buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);
        //创建右拉普拉斯金字塔
        buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);
        //创建高斯金字塔
        buildGaussianPyramid();
    }

    void buildGaussianPyramid() {//金字塔内容为每一层的掩模
        assert(leftLapPyr.size() > 0);

        maskGaussianPyramid.clear();
        Mat currentImg;
        cvtColor(blendMask, currentImg,
                 CV_GRAY2BGR);//store color img of blend mask into maskGaussianPyramid
        maskGaussianPyramid.push_back(currentImg); //0-level

        currentImg = blendMask;
        for (size_t l = 1; l < levels + 1; l++) {
            Mat _down;
            if (leftLapPyr.size() > l)
                pyrDown(currentImg, _down, leftLapPyr[l].size());
            else
                pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level

            Mat down;
            cvtColor(_down, down, CV_GRAY2BGR);
            maskGaussianPyramid.push_back(down);//add color blend mask into mask Pyramid
            currentImg = _down;
        }
    }

    void buildLaplacianPyramid(const Mat &img, vector<Mat_<Vec3f> > &lapPyr, Mat &HighestLevel) {
        lapPyr.clear();
        Mat currentImg = img;
        for (size_t l = 0; l < levels; l++) {
            Mat down, up;
            pyrDown(currentImg, down);
            pyrUp(down, up, currentImg.size());
            Mat lap = currentImg - up;
            lapPyr.push_back(lap);
            currentImg = down;
        }
        currentImg.copyTo(HighestLevel);
    }

    Mat_<Vec3f> reconstructImgFromLapPyramid() {
        //将左右laplacian图像拼成的resultLapPyr金字塔中每一层
        //从上到下插值放大并相加，即得blend图像结果
        Mat currentImg = resultHighestLevel;
        for (int l = levels - 1; l >= 0; l--) {
            Mat up;

            pyrUp(currentImg, up, resultLapPyr[l].size());
            currentImg = up + resultLapPyr[l];
        }
        return currentImg;
    }

    void blendLapPyrs() {
        //获得每层金字塔中直接用左右两图Laplacian变换拼成的图像resultLapPyr
        resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) +
                             rightHighestLevel.mul(
                                     Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
        for (size_t l = 0; l < levels; l++) {
            Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
            Mat antiMask = Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
            Mat B = rightLapPyr[l].mul(antiMask);
            Mat_<Vec3f> blendedLevel = A + B;

            resultLapPyr.push_back(blendedLevel);
        }
    }

public:
    LaplacianBlending(const Mat_<Vec3f> &_left, const Mat_<Vec3f> &_right,
                      const Mat_<float> &_blendMask, int _levels)
            ://construct function, used in LaplacianBlending lb(l,r,m,4);
            left(_left), right(_right), blendMask(_blendMask), levels(_levels) {
        assert(_left.size() == _right.size());
        assert(_left.size() == _blendMask.size());
        //构建拉普拉斯金字塔和高斯金字塔
        buildPyramids();  //construct Laplacian Pyramid and Gaussian Pyramid
        //将左右金字塔混合成一个金字塔
        blendLapPyrs();   //blend left & right Pyramids into one Pyramid
    };

    Mat_<Vec3f> blend() {
        return reconstructImgFromLapPyramid();//reconstruct Image from Laplacian Pyramid
    }
};

Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f> &l, const Mat_<Vec3f> &r, const Mat_<float> &m) {
    LaplacianBlending lb(l, r, m, 4);
    return lb.blend();
}



