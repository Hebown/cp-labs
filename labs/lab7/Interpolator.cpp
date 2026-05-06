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