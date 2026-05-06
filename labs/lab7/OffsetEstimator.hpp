#pragma once
#include "opencv2/core/types.hpp"
#include <vector>
#include <opencv2/core/mat.hpp>

class OffsetEstimator {
public:
    virtual ~OffsetEstimator() = default;
    // 输入：柱面图像序列（已投影）
    // 输出：相邻图像之间的相对偏移（cv::Point2d 存储 dx, dy）
    // 返回 false 表示计算失败
    virtual bool computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                        std::vector<cv::Point2d>& rel_offsets) = 0;
};

class OrbMedianOffsetEstimator : public OffsetEstimator {
public:
    OrbMedianOffsetEstimator(int nfeatures = 5000, float ratio = 0.75f, int min_matches = 10);
    bool computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                std::vector<cv::Point2d>& rel_offsets) override;

private:
    int nfeatures_;
    float ratio_;
    int min_matches_;
};

class OrbRansacOffsetEstimator : public OffsetEstimator {
public:
    OrbRansacOffsetEstimator(int nfeatures = 5000, float ratio = 0.75f, 
                            int min_matches = 10, double ransac_thresh = 3.0);
    
    bool computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                std::vector<cv::Point2d>& rel_offsets) override;

private:
    int nfeatures_;
    float ratio_;
    int min_matches_;
    double ransac_thresh_; // RANSAC 判定为内点的最大距离
};