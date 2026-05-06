#pragma once
#include <vector>
#include <opencv2/core/mat.hpp>

class ImageBlender {
public:
    virtual ~ImageBlender() = default;
    // 输入：柱面图像列表，每张图像在最终画布中的左上角x坐标（y均为0），画布尺寸（预先计算好）
    // 输出：融合后的全景图
    virtual void blend(const std::vector<cv::Mat>& images,
                       const std::vector<cv::Point>& x_offsets,
                       const cv::Size& canvas_size,
                       cv::Mat& result) = 0;
};

class LinearBlender : public ImageBlender {
public:
    void blend(const std::vector<cv::Mat>& images,
               const std::vector<cv::Point>& x_offsets,
               const cv::Size& canvas_size,
               cv::Mat& result) override;
};