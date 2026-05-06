#include "ImageBlender.hpp"

void LinearBlender::blend(const std::vector<cv::Mat>& images,
                          const std::vector<int>& x_offsets,
                          const cv::Size& canvas_size,
                          cv::Mat& result) {
    CV_Assert(images.size() == x_offsets.size());
    result = cv::Mat::zeros(canvas_size, CV_8UC3);
    cv::Mat weight = cv::Mat::zeros(canvas_size, CV_32F);

    for (size_t idx = 0; idx < images.size(); ++idx) {
        const cv::Mat& img = images[idx];
        int x_start = x_offsets[idx];
        int x_end = x_start + img.cols;
        int y_start = 0;
        int y_end = img.rows;

        // 为当前图像生成权重图（从左到右线性渐变，从0到1）
        cv::Mat img_weight = cv::Mat::zeros(img.size(), CV_32F);
        for (int x = 0; x < img.cols; ++x) {
            float w = static_cast<float>(x) / (img.cols - 1);
            for (int y = 0; y < img.rows; ++y) {
                img_weight.at<float>(y, x) = w;
            }
        }

        // 叠加到全景图
        for (int y = y_start; y < y_end; ++y) {
            for (int x = x_start; x < x_end; ++x) {
                int img_x = x - x_start;
                int img_y = y - y_start;
                float w = img_weight.at<float>(img_y, img_x);
                float old_w = weight.at<float>(y, x);
                float new_w = old_w + w;

                if (new_w < 1e-6) continue;

                cv::Vec3b pix = img.at<cv::Vec3b>(img_y, img_x);
                cv::Vec3f old_pix = result.at<cv::Vec3b>(y, x);

                cv::Vec3b blended;
                for (int c = 0; c < 3; ++c) {
                    float val = (old_pix[c] * old_w + pix[c] * w) / new_w;
                    blended[c] = static_cast<uchar>(val);
                }
                result.at<cv::Vec3b>(y, x) = blended;
                weight.at<float>(y, x) = new_w;
            }
        }
    }
}