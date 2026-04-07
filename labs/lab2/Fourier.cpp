#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

class Solution {
public:
    void run() {
        // 1. 创建 512x512 单通道浮点图像，中心放置 20x60 矩形（值为 1.0）
        cv::Mat I = cv::Mat::zeros(512, 512, CV_32FC1);
        cv::Rect roi(256 - 10, 256 - 30, 20, 60);
        I(roi) = 1.0f;

        auto compute_fft = [&](const cv::Mat& src, const std::string& filename, bool shift = false) {
            cv::Mat complex;
            cv::dft(src, complex, cv::DFT_COMPLEX_OUTPUT); 
            if (shift) {
                fftshift(complex, complex);
            }
            std::vector<cv::Mat> planes;
            cv::split(complex, planes);
            cv::Mat mag;
            cv::magnitude(planes[0], planes[1], mag);
            cv::Mat logMag;
            cv::log(mag + 1.0, logMag);
            cv::normalize(logMag, logMag, 255, 0, cv::NORM_MINMAX);
            logMag.convertTo(logMag, CV_8U);
            cv::imwrite(filename, logMag);
            std::cout << "Saved: " << filename << std::endl;
        };

        auto compute_fft_twice = [&](const cv::Mat& src, const std::string& filename, bool shift = false) {
            cv::Mat J;
            cv::dft(src, J, cv::DFT_COMPLEX_OUTPUT);
            cv::Mat J2;
            cv::dft(J, J2, cv::DFT_COMPLEX_OUTPUT);
            if (shift) {
                fftshift(J2, J2);
            }
            std::vector<cv::Mat> planes;
            cv::split(J2, planes);
            cv::Mat mag;
            cv::magnitude(planes[0], planes[1], mag);
            cv::Mat logMag;
            cv::log(mag + 1.0, logMag);
            cv::normalize(logMag, logMag, 255, 0, cv::NORM_MINMAX);
            logMag.convertTo(logMag, CV_8U);
            cv::imwrite(filename, logMag);
            std::cout << "Saved: " << filename << std::endl;
        };

        // 原始矩形做一次 DFT 与二次 DFT
        compute_fft(I, "Fourier.png", true);
        compute_fft_twice(I, "dft_again.png");

        // 2. 旋转矩形
        cv::Mat rotate_raw = cv::Mat::zeros(512, 512, CV_32FC1);
        cv::Rect rotate_roi(256 - 30, 256 - 10, 60, 20);
        rotate_raw(rotate_roi) = 1.0f;
        compute_fft(rotate_raw, "rotate_result.png", true);
        compute_fft_twice(rotate_raw, "rotate_dft_again.png");

        // 3. 平移矩形（向左上偏移 50 像素）
        cv::Mat move_raw = cv::Mat::zeros(512, 512, CV_32FC1);
        cv::Rect move_roi(256 - 10 - 50, 256 - 30 - 50, 20, 60);
        move_raw(move_roi) = 1.0f;
        compute_fft(move_raw, "move_result.png", true);
        compute_fft_twice(move_raw, "move_dft_again.png");

        // 4. 缩放矩形（长宽变为 40x120）
        cv::Mat size_raw = cv::Mat::zeros(512, 512, CV_32FC1);
        cv::Rect size_roi(256 - 20, 256 - 60, 40, 120);
        size_raw(size_roi) = 1.0f;
        compute_fft(size_raw, "size_result.png", true);
        compute_fft_twice(size_raw, "size_dft_again.png");
    }

private:
    void fftshift(const cv::Mat &src, cv::Mat &dst) {
        dst.create(src.size(), src.type());
        int rows = src.rows, cols = src.cols;
        cv::Rect roiTopBand, roiBottomBand, roiLeftBand, roiRightBand;
        if (rows % 2 == 0) {
            roiTopBand = cv::Rect(0, 0, cols, rows / 2);
            roiBottomBand = cv::Rect(0, rows / 2, cols, rows / 2);
        } else {
            roiTopBand = cv::Rect(0, 0, cols, rows / 2 + 1);
            roiBottomBand = cv::Rect(0, rows / 2 + 1, cols, rows / 2);
        }
        if (cols % 2 == 0) {
            roiLeftBand = cv::Rect(0, 0, cols / 2, rows);
            roiRightBand = cv::Rect(cols / 2, 0, cols / 2, rows);
        } else {
            roiLeftBand = cv::Rect(0, 0, cols / 2 + 1, rows);
            roiRightBand = cv::Rect(cols / 2 + 1, 0, cols / 2, rows);
        }
        cv::Mat srcTopBand = src(roiTopBand);
        cv::Mat dstTopBand = dst(roiTopBand);
        cv::Mat srcBottomBand = src(roiBottomBand);
        cv::Mat dstBottomBand = dst(roiBottomBand);
        cv::Mat srcLeftBand = src(roiLeftBand);
        cv::Mat dstLeftBand = dst(roiLeftBand);
        cv::Mat srcRightBand = src(roiRightBand);
        cv::Mat dstRightBand = dst(roiRightBand);
        flip(srcTopBand, dstTopBand, 0);
        flip(srcBottomBand, dstBottomBand, 0);
        flip(dst, dst, 0);
        flip(srcLeftBand, dstLeftBand, 1);
        flip(srcRightBand, dstRightBand, 1);
        flip(dst, dst, 1);
    }
};

int main() {
    try {
        Solution solution;
        solution.run();
    } catch (std::exception& e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}