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

class BicubicInterpolator : public Interpolator {
public:
    cv::Vec3b sample(const cv::Mat& image, double x, double y) const override;
private:
    // 三次多项式权重函数
    static inline double cubicWeight(double x) {
        double a = -0.5; 
        double absX = std::abs(x);
        if (absX <= 1.0) 
            return (a + 2.0) * absX * absX * absX - (a + 3.0) * absX * absX + 1.0;
        if (absX < 2.0) 
            return a * absX * absX * absX - 5.0 * a * absX * absX + 8.0 * a * absX - 4.0 * a;
        return 0.0;
    }
};