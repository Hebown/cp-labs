#include <opencv2/opencv.hpp>
#include <iostream>

/*
    此文件用于我生成测试图样
*/

int main() {
    const int size = 512;
    cv::Mat img(size, size, CV_8UC3, cv::Scalar(0, 0, 0));

    // 1. 背景水平渐变 (BGR 三通道同步变化，得到灰度渐变)
    for (int j = 0; j < size; ++j) {
        uchar val = static_cast<uchar>(255 * j / size);  // 从左到右 0→255
        img.col(j).setTo(cv::Scalar(val, val, val));
    }

    // 2. 添加白色矩形 (强边缘)
    cv::rectangle(img, cv::Rect(100, 100, 100, 100), cv::Scalar(255, 255, 255), cv::FILLED);

    // 3. 添加黑色圆形 (强边缘)
    cv::circle(img, cv::Point(400, 150), 70, cv::Scalar(0, 0, 0), cv::FILLED);

    // 4. 棋盘纹理区域 (高频细节)
    int chess_size = 20;
    for (int i = 300; i < 450; i += chess_size) {
        for (int j = 300; j < 450; j += chess_size) {
            bool is_white = ((i / chess_size) + (j / chess_size)) % 2 == 0;
            uchar color = is_white ? 255 : 0;
            cv::Rect roi(j, i, chess_size, chess_size);
            img(roi) = cv::Scalar(color, color, color);
        }
    }

    // 5. 添加高斯噪声 (在整张图像上)
    cv::Mat noise = cv::Mat::zeros(img.size(), CV_32FC3);
    cv::randn(noise, cv::Scalar::all(0), cv::Scalar::all(25));  // 均值为0，标准差25
    cv::Mat img_float;
    img.convertTo(img_float, CV_32FC3);
    img_float += noise;
    img_float.convertTo(img, CV_8UC3);

    // 保存图像
    cv::imwrite("test_image.png", img);
    std::cout << "测试图像已生成: test_image.png" << std::endl;
    return 0;
}