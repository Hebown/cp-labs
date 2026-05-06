#pragma once
#include <opencv2/core/mat.hpp>

class Interpolator {
public:
    virtual ~Interpolator() = default;
    // 从单通道或三通道图像中采样 (x, y) 处的像素值，坐标可为浮点
    virtual cv::Vec3b sample(const cv::Mat& image, double x, double y) const = 0;
};

class NearestInterpolator : public Interpolator {
public:
    cv::Vec3b sample(const cv::Mat& image, double x, double y) const override;
};

class BilinearInterpolator : public Interpolator {
public:
    cv::Vec3b sample(const cv::Mat& image, double x, double y) const override;
};