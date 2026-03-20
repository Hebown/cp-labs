#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include "utils.hpp"

int main(int argc, char** argv) {
    try {
        if (argc != 5) {
            throw std::runtime_error("参数数量错误，期望获得四个参数");
        }
        std::string inputFilePath = argv[1], outputFilePath = argv[2];
        double sigma_s = std::stod(argv[3]);
        double sigma_r = std::stod(argv[4]);
        int size = static_cast<int>(std::floor(sigma_s * 5));

        // --- 预计算空间高斯权重表 ---
        int kernel_size = 2 * size + 1;
        std::vector<std::vector<double>> space_weight(kernel_size,
                                                       std::vector<double>(kernel_size));
        double sigma_s_sq = sigma_s * sigma_s;
        for (int ix = -size; ix <= size; ++ix) {
            for (int iy = -size; iy <= size; ++iy) {
                double dist2 = static_cast<double>(ix * ix + iy * iy);
                space_weight[ix + size][iy + size] = std::exp(-dist2 / (2.0 * sigma_s_sq));
            }
        }

        Solution solution;
        solution.run(inputFilePath, outputFilePath, size, size,
            [&](const cv::Mat& image, int i, int j) {
                const cv::Vec3b& center = image.at<cv::Vec3b>(i, j);
                double sum_weight = 0.0;
                double real_weight[3] = {0.0, 0.0, 0.0};

                for (int ix = -size; ix <= size; ++ix) {
                    for (int iy = -size; iy <= size; ++iy) {
                        double space_w = space_weight[ix + size][iy + size];

                        const cv::Vec3b& q = image.at<cv::Vec3b>(ix + i, iy + j);
                        double color_diff = (q[0] - center[0]) * (q[0] - center[0]) +
                                            (q[1] - center[1]) * (q[1] - center[1]) +
                                            (q[2] - center[2]) * (q[2] - center[2]);
                        double color_w = std::exp(-color_diff / (2.0 * sigma_r * sigma_r));

                        double weight = space_w * color_w;
                        sum_weight += weight;
                        for (int c = 0; c < 3; ++c) {
                            real_weight[c] += weight * q[c];
                        }
                    }
                }

                cv::Vec3b result;
                for (int c = 0; c < 3; ++c) {
                    double val = real_weight[c] / sum_weight;
                    int ival = static_cast<int>(std::round(val));
                    result[c] = cv::saturate_cast<uchar>(ival);
                }
                return result;
            }
        );
    } catch (std::exception& e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
    }
}