#include "Panorama.hpp"
#include "CylindricalProjector.hpp"
#include <cstdio>

bool Panorama::makePanorama(std::vector<cv::Mat>& img_vec, cv::Mat& img_out, double f) {
    // 1. 将所有图像投影到柱面
    std::vector<cv::Mat> cyl_images;
    cyl_images.reserve(img_vec.size());
    for (size_t i = 0; i < img_vec.size(); ++i) {
        cv::Mat cyl;
        CylindricalProjector::project(img_vec[i], f, cyl, *interpolatorPtr);
        if (cyl.empty()) {
            printf("警告：第 %zu 张图像柱面投影失败\n", i);
            return false;
        }
        cyl_images.push_back(cyl);
        printf("第 %zu 张图像柱面投影完成，尺寸 %dx%d\n", i, cyl.cols, cyl.rows);
    }

    // 2. 计算相邻图像的水平偏移
    std::vector<double> rel_offsets;
    if (!offsetEstimatorPtr->computeRelativeOffsets(cyl_images, rel_offsets)) {
        printf("偏移计算失败\n");
        return false;
    }

    // 累积全局偏移
    std::vector<double> offsets(cyl_images.size(), 0.0);
    for (size_t i = 1; i < cyl_images.size(); ++i) {
        offsets[i] = offsets[i-1] + rel_offsets[i-1];
    }

    // 3. 根据偏移量确定画布尺寸和每张图的放置位置
    if (cyl_images.empty()) return false;
    double min_offset = *std::min_element(offsets.begin(), offsets.end());
    int canvas_height = 0;
    for (const auto& img : cyl_images) {
        canvas_height = std::max(canvas_height, img.rows);
    }
    std::vector<int> x_positions(cyl_images.size());
    double max_right = 0.0;
    for (size_t i = 0; i < cyl_images.size(); ++i) {
        x_positions[i] = static_cast<int>(std::round(offsets[i] - min_offset));
        double right = x_positions[i] + cyl_images[i].cols;
        max_right = std::max(max_right, right);
    }
    int canvas_width = static_cast<int>(std::ceil(max_right));
    cv::Size canvas_size(canvas_width, canvas_height);

    cv::Mat panorama;
    imageBlenderPtr->blend(cyl_images, x_positions, canvas_size, panorama);
    img_out = panorama;
    return true;
}