#include "OffsetEstimator.hpp"
#include <opencv2/features2d.hpp>
#include <algorithm>
#include <cstdio>

OrbMedianOffsetEstimator::OrbMedianOffsetEstimator(int nfeatures, float ratio, int min_matches)
    : nfeatures_(nfeatures), ratio_(ratio), min_matches_(min_matches) {}

bool OrbMedianOffsetEstimator::computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                                      std::vector<double>& rel_offsets) {
    rel_offsets.clear();
    if (cyl_images.size() < 2) return false;

    cv::Ptr<cv::ORB> orb = cv::ORB::create(nfeatures_);
    cv::BFMatcher bf(cv::NORM_HAMMING);

    for (size_t i = 0; i < cyl_images.size() - 1; ++i) {
        cv::Mat img1 = cyl_images[i];
        cv::Mat img2 = cyl_images[i+1];

        std::vector<cv::KeyPoint> kp1, kp2;
        cv::Mat desc1, desc2;
        orb->detectAndCompute(img1, cv::noArray(), kp1, desc1);
        orb->detectAndCompute(img2, cv::noArray(), kp2, desc2);

        if (desc1.empty() || desc2.empty()) {
            printf("警告：图像 %zu 或 %zu 特征提取失败\n", i, i+1);
            rel_offsets.push_back(0.0);
            continue;
        }

        std::vector<std::vector<cv::DMatch>> matches;
        bf.knnMatch(desc1, desc2, matches, 2);

        std::vector<cv::DMatch> good_matches;
        for (auto& m : matches) {
            if (m.size() == 2 && m[0].distance < ratio_ * m[1].distance) {
                good_matches.push_back(m[0]);
            }
        }
        if (good_matches.size() < min_matches_) {
            printf("警告：图像 %zu 和 %zu 匹配点太少 (%zu)\n", i, i+1, good_matches.size());
            rel_offsets.push_back(0.0);
            continue;
        }

        std::vector<double> deltas;
        for (const auto& match : good_matches) {
            double x1 = kp1[match.queryIdx].pt.x;
            double x2 = kp2[match.trainIdx].pt.x;
            deltas.push_back(x1 - x2);   // 正值表示 img2 需要向右移
        }
        std::sort(deltas.begin(), deltas.end());
        double median_dx = deltas[deltas.size() / 2];
        rel_offsets.push_back(median_dx);
        printf("图像 %zu -> %zu 水平偏移: %.2f 像素\n", i, i+1, median_dx);
    }
    return true;
}