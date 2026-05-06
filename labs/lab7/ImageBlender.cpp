#include "ImageBlender.hpp"
#include "opencv2/core/types.hpp"

void LinearBlender::blend(const std::vector<cv::Mat>& images,
                          const std::vector<cv::Point>& offsets,
                          const cv::Size& canvas_size,
                          cv::Mat& result) {
    CV_Assert(images.size() == offsets.size());
    result = cv::Mat::zeros(canvas_size, CV_8UC3);
    
    cv::Mat acc_result = cv::Mat::zeros(canvas_size, CV_32FC3);
    cv::Mat acc_weight = cv::Mat::zeros(canvas_size, CV_32F);

    // 修改模糊的宽度
    const float feather_width = 40.0f; 

    for (size_t i = 0; i < images.size(); ++i) {
        const cv::Mat& img = images[i];
        int x_start = offsets[i].x;
        int y_start = offsets[i].y;
        
        // 计算与左右邻居的重叠宽度
        int overlap_left = (i == 0) ? 0 : (offsets[i-1].x + images[i-1].cols - x_start);
        int overlap_right = (i == images.size() - 1) ? 0 : (x_start + img.cols - offsets[i+1].x);

        for (int y = 0; y < img.rows; ++y) {
            for (int x = 0; x < img.cols; ++x) {
                float w = 1.0f;

                // 窄带权重计算逻辑
                // 1. 处理左边缘（进入带）
                if (i > 0 && x < overlap_left) {
                    float center = overlap_left / 2.0f;
                    float start_f = center - feather_width / 2.0f;
                    float end_f = center + feather_width / 2.0f;

                    if (x < start_f) w = 0.0f;
                    else if (x > end_f) w = 1.0f;
                    else w = (x - start_f) / feather_width;
                }
                
                // 2. 处理右边缘（退出带）
                // 只有当 w 还没被左边缘清零时才计算右边缘
                if (i < images.size() - 1 && x > (img.cols - overlap_right)) {
                    float local_x = (float)x - (img.cols - overlap_right);
                    float center = overlap_right / 2.0f;
                    float start_f = center - feather_width / 2.0f;
                    float end_f = center + feather_width / 2.0f;

                    float right_w = 1.0f;
                    if (local_x < start_f) right_w = 1.0f;
                    else if (local_x > end_f) right_w = 0.0f;
                    else right_w = 1.0f - (local_x - start_f) / feather_width;
                    
                    w = std::min(w, right_w);
                }

                int canvas_x = x_start + x;
                int canvas_y = y_start + y;

                if (canvas_x < 0 || canvas_x >= canvas_size.width || 
                    canvas_y < 0 || canvas_y >= canvas_size.height) continue;

                cv::Vec3f pix = img.at<cv::Vec3b>(y, x);
                acc_result.at<cv::Vec3f>(canvas_y, canvas_x) += pix * w;
                acc_weight.at<float>(canvas_y, canvas_x) += w;
            }
        }
    }

    // 归一化输出
    for (int y = 0; y < canvas_size.height; ++y) {
        for (int x = 0; x < canvas_size.width; ++x) {
            float total_w = acc_weight.at<float>(y, x);
            if (total_w > 1e-6) {
                acc_result.at<cv::Vec3f>(y, x) /= total_w;
            }
        }
    }
    acc_result.convertTo(result, CV_8UC3);
}
