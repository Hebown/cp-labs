#include "Panorama.hpp"
#include "CylindricalProjector.hpp"
#include <cstdio>

bool Panorama::makePanorama(std::vector<cv::Mat>& img_vec, cv::Mat& img_out, double f) {
    // 1. 将所有图像投影到柱面
    std::vector<cv::Mat> cyl_images;
    cyl_images.reserve(img_vec.size());
    for (size_t i = 0; i < img_vec.size(); ++i) {        
        // 柱面投影
        cv::Mat cyl;
        CylindricalProjector::project(img_vec[i], f, cyl, *interpolatorPtr);
        if (cyl.empty()) {
            printf("警告：第 %zu 张图像柱面投影失败\n", i);
            return false;
        }
        cyl_images.push_back(cyl);
        printf("第 %zu 张图像柱面投影完成，尺寸 %dx%d\n", i, cyl.cols, cyl.rows);
    }

    // 2. 计算相对偏移
    std::vector<cv::Point2d> rel_offsets;
    offsetEstimatorPtr->computeRelativeOffsets(cyl_images, rel_offsets);

    // 累积全局偏移
    std::vector<cv::Point2d> abs_offsets(cyl_images.size(), cv::Point2d(0, 0));
    double min_y = 0, max_y = 0, max_x = 0;

    for (size_t i = 1; i < cyl_images.size(); ++i) {
        abs_offsets[i] = abs_offsets[i-1] + rel_offsets[i-1];
        min_y = std::min(min_y, abs_offsets[i].y);
    }

    // 计算画布尺寸并校正坐标使其全为正数
    std::vector<cv::Point> final_pos(cyl_images.size());
    double min_x = 0; 
    for(auto& p : abs_offsets) min_x = std::min(min_x, p.x);

    for (size_t i = 0; i < cyl_images.size(); ++i) {
        final_pos[i].x = static_cast<int>(std::round(abs_offsets[i].x - min_x));
        final_pos[i].y = static_cast<int>(std::round(abs_offsets[i].y - min_y));
        
        max_x = std::max(max_x, (double)final_pos[i].x + cyl_images[i].cols);
        max_y = std::max(max_y, (double)final_pos[i].y + cyl_images[i].rows);
    }

    cv::Size canvas_size(static_cast<int>(std::ceil(max_x)), static_cast<int>(std::ceil(max_y)));

    // 3. 融合
    imageBlenderPtr->blend(cyl_images, final_pos, canvas_size, img_out);
    return true;
}