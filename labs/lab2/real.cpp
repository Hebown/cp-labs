#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <opencv2/opencv.hpp>

/*
    此文件用于我对照双边滤波的结果是否正确，可忽略
*/

int main(int argc, char** argv){
    try {
        if(argc!=5){
            throw std::runtime_error("参数数量错误，期望获得四个参数");
        }
        std::string inputFilePath=argv[1],outputFilePath=argv[2];
        double sigma_s=std::stod(argv[3]);
        double sigma_r=std::stod(argv[4]);
        int size = static_cast<int>(std::floor(sigma_s * 5));
        int d = 2 * size + 1;  // 滤波核直径

        cv::Mat image = cv::imread(inputFilePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            throw std::runtime_error("无法读取图像，请检查文件路径");
        }

        cv::Mat result;
        cv::bilateralFilter(image, result, d, sigma_r, sigma_s);

        cv::imwrite(outputFilePath, result);
        std::cout << "处理完成，结果已保存至 " << outputFilePath << std::endl;
    } catch (std::exception& e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
    }
}