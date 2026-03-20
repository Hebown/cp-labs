#include "opencv2/core.hpp"
#include "opencv2/core/base.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include <exception>
#include <iostream>
#include <vector>

class Solution{
public:
    void run(){
        cv::Mat I(512, 512, CV_32FC1, cv::Scalar(0));
        cv::Rect roi(256-10, 256-30, 20, 60);
        I(roi) = 1.0;
        cv::Mat J;
        cv::dft(I, J, cv::DFT_COMPLEX_OUTPUT);
        fftshift(J, J);
        cv::Mat Mag;
        std::vector<cv::Mat> K;
        cv::split(J, K);
        cv::pow(K[0], 2, K[0]);
        cv::pow(K[1], 2, K[1]);
        Mag = K[0] + K[1];
        cv::Mat logMag;
        cv::log(Mag + 1, logMag);
        cv::normalize(logMag, logMag, 1.0, 0.0, cv::NORM_MINMAX);
        cv::imshow("Magnitude", logMag);
        cv::waitKey();
    }

private:
    void fftshift(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.size(), src.type());
        int rows = src.rows, cols = src.cols;
        cv::Rect roiTopBand, roiBottomBand, roiLeftBand, roiRightBand;
        if  (rows %  2  ==  0) {
            roiTopBand =  cv::Rect(0,  0, cols, rows /  2);
            roiBottomBand =  cv::Rect(0, rows /  2, cols, rows /  2);
        } else {
            roiTopBand =  cv::Rect(0,  0, cols, rows /  2  +  1);
            roiBottomBand =  cv::Rect(0, rows /  2  +  1, cols, rows /  2);
        }
        if  (cols %  2  ==  0)  {
            roiLeftBand =  cv::Rect(0,  0, cols /  2, rows);
            roiRightBand =  cv::Rect(cols /  2,  0, cols /  2, rows);
        }  else  {
            roiLeftBand =  cv::Rect(0,  0, cols /  2  +  1, rows);
            roiRightBand =  cv::Rect(cols /  2  +  1,  0, cols /  2, rows);
        }
        cv::Mat srcTopBand =  src(roiTopBand);
        cv::Mat dstTopBand =  dst(roiTopBand);
        cv::Mat srcBottomBand =  src(roiBottomBand);
        cv::Mat dstBottomBand =  dst(roiBottomBand);
        cv::Mat srcLeftBand =  src(roiLeftBand);
        cv::Mat dstLeftBand =  dst(roiLeftBand);
        cv::Mat srcRightBand =  src(roiRightBand);
        cv::Mat dstRightBand =  dst(roiRightBand);
        flip(srcTopBand, dstTopBand,  0);
        flip(srcBottomBand, dstBottomBand,  0);
        flip(dst, dst,  0);
        flip(srcLeftBand, dstLeftBand,  1);
        flip(srcRightBand, dstRightBand,  1);
        flip(dst, dst,  1);
}
};

int main(){
    try {
        Solution solution;
        solution.run();
    } catch (std::exception& e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
    }
    return 0;
}