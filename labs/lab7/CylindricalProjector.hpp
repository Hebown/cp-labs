#pragma once
#include "Interpolator.hpp"
#include <opencv2/core/mat.hpp>

class CylindricalProjector{
public:
    // 柱面坐标 -> 原始图像坐标
    static cv::Point2d backward(double xp, double yp, double f, double cx, double cy);

    // 将原始图像投影到柱面，生成柱面图像
    // src: 原始图像
    // f: 焦距（像素）
    // dst: 输出柱面图像
    static void project(const cv::Mat& src, double f, cv::Mat& dst, const Interpolator& interpolator);
};