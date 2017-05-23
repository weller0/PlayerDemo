/*
 * LaplacianBlending.cpp
 *
 *  Created on: 2017-3-29
 *      Author: lyz1900
 */

#include"LaplacianBlending.h"

class LaplacianBlending_YUV420P {
private:
    Mat_<float> left;
    Mat_<float> right;
    Mat_<float> blendMask;

    vector<Mat_<float> > leftLapPyr, rightLapPyr, resultLapPyr;    //拉普拉斯金字塔	Laplacian Pyramids
    Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;    //金字塔最高层
    vector<Mat_<float> > maskGaussianPyramid;                            //高斯金字塔 Mask （改为单通道）

    size_t levels;

    /*创建金字塔	创建左拉普拉斯金字塔、右拉普拉斯金字塔、高斯金字塔掩模*********************/
    void buildPyramids() {
        //创建左拉布拉斯金字塔
        buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);

        //创建右拉普拉斯金字塔
        buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);

        //创建高斯金字塔,并为每一层掩模
        buildGaussianPyramid();
    }

    /*创建高斯金字塔，并为每一层掩模*/
    void buildGaussianPyramid() {
        assert(leftLapPyr.size() > 0);        //断言，判断左拉普拉斯金字塔已经创建好

        maskGaussianPyramid.clear();    //清空高斯金字塔掩模 vector

        Mat currentImg;        //当前图像
        maskGaussianPyramid.push_back(blendMask);     //0-level 高斯金字塔掩模 第 0 层，即为原始模板
        currentImg = blendMask;

        for (size_t l = 1; l < levels + 1; l++) {
            Mat _down;
            if (leftLapPyr.size() > l)
                pyrDown(currentImg, _down,
                        leftLapPyr[l].size());        //生成模板金字塔，模板每层大小与 左拉普拉斯金字塔每层一致
            else
                pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level

            maskGaussianPyramid.push_back(
                    _down);                        //add color blend mask into mask Pyramid
            currentImg = _down;
        }
    }

    /*创建拉普拉斯金字塔*******************************************************************************
     * input	 	const Mat& img 			原图
     * output  	vector<Mat_<float> > 	拉普拉斯金字塔 （vector of Mat）
     * output	Mat& HighestLevel 		金字塔最高层***********************************************************/
    void buildLaplacianPyramid(const Mat &img, vector<Mat_<float> > &lapPyr, Mat &HighestLevel) {
        lapPyr.clear();                    //清除 vector
        Mat currentImg = img;    //第一次以原图做为开始缩放
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

    /*从经过合并的拉普拉斯金字塔 resultLapPyr 重建图像*************************************************************************/
    Mat_<float> reconstructImgFromLapPyramid() {
        //将左右laplacian图像拼成的resultLapPyr金字塔中每一层
        //从上到下插值放大并相加，即得blend图像结果
        Mat currentImg = resultHighestLevel;        //最顶层用合并的拉普拉斯金字塔顶层
        for (int l = levels - 1; l >= 0; l--) {
            Mat up;
            pyrUp(currentImg, up, resultLapPyr[l].size());    //从上到下插值放大，放大的大小与 resultLapPyr 的每一层相同
            currentImg =
                    up + resultLapPyr[l];                        //将插值放大的图像与 resultLapPyr 相加，得到输出图像
        }
        return currentImg;
    }

    /*左右拉普拉斯金字塔合并***************************************************************************
     * 获得每层金字塔中直接用左右两图Laplacian变换拼成的图像resultLapPyr
     *
     * */
    void blendLapPyrs() {
        //融合最高层
        resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) +
                             rightHighestLevel.mul(1.0 - maskGaussianPyramid.back());
        //融合每一层，		左边金字塔 × mask ，右边金字塔 × （1-mask）
        for (size_t l = 0; l < levels; l++) {
            Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);

            Mat antiMask = 1.0 - maskGaussianPyramid[l];

            Mat B = rightLapPyr[l].mul(antiMask);

            Mat_<float> blendedLevel = A + B;

            resultLapPyr.push_back(blendedLevel);
        }
    }

public:
    LaplacianBlending_YUV420P(const Mat_<float> &_left, const Mat_<float> &_right,
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

    // 重构图像
    Mat_<float> blend_YUV420P() {
        return reconstructImgFromLapPyramid();//reconstruct Image from Laplacian Pyramid
    }
};

Mat_<float> LaplacianBlend_YUV420P(const Mat_<float> &l, const Mat_<float> &r,
                                   const Mat_<float> &m) {
    LaplacianBlending_YUV420P lb(l, r, m, 3);        //如果是小融合，为 2，如果是大融合，4
    return lb.blend_YUV420P();
}