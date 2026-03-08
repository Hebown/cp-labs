#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"  // 用于显示图像
#include <iostream>

int main(){
        
    cv::Mat image = cv::imread("opencv-logo.png");
    
    // 图像的通道数
    std::cout << "image channels = " << image.channels() << std::endl;
    std::cout << "channel type = " << image.type() << std::endl; // 输出 16，表示 8比特 无符号数 + 3通道
    
    cv::Vec3b _pixel = image.at<cv::Vec3b>(0,0); // 使用 Vec3 byte 访问每个像素
    std::cout << "image rows (height) = " << image.rows << std::endl; // 图像的高度
    std::cout << "image cols (width) = " << image.cols << std::endl; // 图像的宽度
    
    // 反色，一种方法是 使用 Scalar
    cv::Mat result = cv::Scalar(255, 255, 255) - image;
    
    cv::imwrite("opencv-logo-inverted-scalar.png", result);

    cv::waitKey(0); // 等待按键
    return 0;
}