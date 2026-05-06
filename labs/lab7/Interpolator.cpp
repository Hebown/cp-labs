#include "Interpolator.hpp"
#include <cmath>

cv::Vec3b NearestInterpolator::sample(const cv::Mat& image, double x, double y) const {
    int ix = static_cast<int>(std::round(x));
    int iy = static_cast<int>(std::round(y));
    if (ix < 0 || ix >= image.cols || iy < 0 || iy >= image.rows)
        return cv::Vec3b(0, 0, 0);
    return image.at<cv::Vec3b>(iy, ix);
}

cv::Vec3b BilinearInterpolator::sample(const cv::Mat& image, double x, double y) const {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x0 < 0 || x1 >= image.cols || y0 < 0 || y1 >= image.rows)
        return cv::Vec3b(0, 0, 0);

    double dx = x - x0;
    double dy = y - y0;

    cv::Vec3b v00 = image.at<cv::Vec3b>(y0, x0);
    cv::Vec3b v10 = image.at<cv::Vec3b>(y0, x1);
    cv::Vec3b v01 = image.at<cv::Vec3b>(y1, x0);
    cv::Vec3b v11 = image.at<cv::Vec3b>(y1, x1);

    cv::Vec3b result;
    for (int c = 0; c < 3; ++c) {
        double top = (1 - dx) * v00[c] + dx * v10[c];
        double bottom = (1 - dx) * v01[c] + dx * v11[c];
        double val = (1 - dy) * top + dy * bottom;
        result[c] = static_cast<uchar>(val);
    }
    return result;
}

cv::Vec3b BicubicInterpolator::sample(const cv::Mat& image, double x, double y) const {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));

    // 双三次插值需要周围 4x4 的像素窗口
    // 边界检查：确保窗口 [x0-1, x0+2] 和 [y0-1, y0+2] 都在图像范围内
    if (x0 < 1 || x0 >= image.cols - 2 || y0 < 1 || y0 >= image.rows - 2) {
        // 边缘退化处理：如果靠近边缘，回退到双线性或简单裁剪
        if (x0 < 0 || x0 >= image.cols || y0 < 0 || y0 >= image.rows)
            return cv::Vec3b(0, 0, 0);
        return image.at<cv::Vec3b>(y0, x0);
    }

    double dx = x - x0;
    double dy = y - y0;

    cv::Vec3f result(0, 0, 0);
    
    // 计算 y 方向的 4 个权重
    double wy[4];
    for (int j = 0; j < 4; ++j) wy[j] = cubicWeight(dy - (j - 1));

    // 计算 x 方向的 4 个权重
    double wx[4];
    for (int i = 0; i < 4; ++i) wx[i] = cubicWeight(dx - (i - 1));

    // 4x4 窗口加权累加
    for (int j = 0; j < 4; ++j) {
        int iy = y0 + j - 1;
        for (int i = 0; i < 4; ++i) {
            int ix = x0 + i - 1;
            double weight = wx[i] * wy[j];
            cv::Vec3b pixel = image.at<cv::Vec3b>(iy, ix);
            result += cv::Vec3f(pixel) * static_cast<float>(weight);
        }
    }

    // 最终色彩值裁剪到 [0, 255]
    return cv::Vec3b(
        static_cast<uchar>(std::max(0.0f, std::min(255.0f, result[0]))),
        static_cast<uchar>(std::max(0.0f, std::min(255.0f, result[1]))),
        static_cast<uchar>(std::max(0.0f, std::min(255.0f, result[2])))
    );
}