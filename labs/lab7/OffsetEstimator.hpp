#pragma once
#include <vector>
#include <opencv2/core/mat.hpp>

class OffsetEstimator {
public:
    virtual ~OffsetEstimator() = default;
    // 输入：柱面图像序列（已投影）
    // 输出：相邻图像之间的相对水平偏移（第二张相对于第一张需要移动的像素，正值表示向左）
    // 返回 false 表示计算失败
    virtual bool computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                        std::vector<double>& rel_offsets) = 0;
};

class OrbMedianOffsetEstimator : public OffsetEstimator {
public:
    OrbMedianOffsetEstimator(int nfeatures = 5000, float ratio = 0.75f, int min_matches = 10);
    bool computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                std::vector<double>& rel_offsets) override;

private:
    int nfeatures_;
    float ratio_;
    int min_matches_;
};