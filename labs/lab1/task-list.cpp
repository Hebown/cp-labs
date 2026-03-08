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
    
    // 遍历每个像素
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // 获取当前像素
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            
            // 判断是否为白色（BGR三个通道都为255）
            // 白色在BGR中为 (255, 255, 255)
            if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
                // 将白色修改为黑色 (0, 0, 0)
                image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    
    // 保存修改后的图像
    cv::imwrite("opencv-logo-modified.png", image);

    cv::waitKey(0); // 等待按键
    return 0;
}