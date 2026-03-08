#include "opencv2/core/hal/interface.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"

int main(){
    
    constexpr int N=32;
    cv::Mat M = cv::Mat::zeros(N, N, CV_64F); // 为了求逆，需要用double
    for (int i = 0; i < N; i++) {
        M.at<double>(i, i) = 2;
        if (i > 0) M.at<double>(i, i-1) = -1;
        if (i < N-1) M.at<double>(i, i+1) = -1;
    }

    // 求逆
    cv::Mat M_inv=M.inv();

    // 转换为灰度图
    double minVal, maxVal; 
    cv::minMaxLoc(M_inv, &minVal, &maxVal); // 获取数值范围   
    cv::Mat grayImage(N, N, CV_8U);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double val = M_inv.at<double>(i, j);
            grayImage.at<uchar>(i, j) = static_cast<uchar>(
                255.0 * (val - minVal) / (maxVal - minVal)
            );
        }
    }
    
    // 保存
    cv::imwrite("M_inv.png", grayImage);
    return 0;
}