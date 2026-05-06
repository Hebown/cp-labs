#include "CylindricalProjector.hpp"
#include <cmath>
#include <opencv2/core/mat.hpp>

cv::Point2d CylindricalProjector::backward(double xp, double yp, double f, double cx, double cy) {
    double xp_centered = xp - cx;
    double yp_centered = yp - cy;

    double theta = xp_centered / f;
    double x_centered = f * tan(theta);
    double y_centered = yp_centered * sqrt(x_centered * x_centered + f * f) / f;

    return cv::Point2d(x_centered + cx, y_centered + cy);
}

void CylindricalProjector::project(const cv::Mat& src, double f, cv::Mat& dst, const Interpolator& interpolator) {
    CV_Assert(!src.empty() && f > 0);
    int src_w = src.cols, src_h = src.rows;
    double cx = (src_w - 1) / 2.0;
    double cy = (src_h - 1) / 2.0;

    // 计算柱面图像尺寸
    double hfov_rad = 2.0 * atan(src_w / (2.0 * f));
    int dst_w = static_cast<int>(f * hfov_rad);
    int dst_h = src_h;

    // 反向映射：遍历柱面每个像素，找到原始图像对应位置并插值
    dst = cv::Mat::zeros(dst_h, dst_w, src.type());

    for (int xp = 0; xp < dst_w; ++xp) {
        for (int yp = 0; yp < dst_h; ++yp) {
            cv::Point2d orig = backward(xp, yp, f, cx, cy);
            dst.at<cv::Vec3b>(yp, xp) = interpolator.sample(src, orig.x, orig.y);
        }
    }
}