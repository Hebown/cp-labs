#include "OffsetEstimator.hpp"
#include <opencv2/features2d.hpp>
#include <algorithm>
#include <cstdio>

OrbMedianOffsetEstimator::OrbMedianOffsetEstimator(int nfeatures, float ratio, int min_matches)
    : nfeatures_(nfeatures), ratio_(ratio), min_matches_(min_matches) {}

bool OrbMedianOffsetEstimator::computeRelativeOffsets(const std::vector<cv::Mat>& cyl_images,
                                                      std::vector<cv::Point2d>& rel_offsets) {
    rel_offsets.clear();
    if (cyl_images.size() < 2) return false;

    cv::Ptr<cv::ORB> orb = cv::ORB::create(nfeatures_);
    cv::BFMatcher bf(cv::NORM_HAMMING);

    for (size_t i = 0; i < cyl_images.size() - 1; ++i) {
        std::vector<cv::KeyPoint> kp1, kp2;
        cv::Mat desc1, desc2;
        orb->detectAndCompute(cyl_images[i], cv::noArray(), kp1, desc1);
        orb->detectAndCompute(cyl_images[i+1], cv::noArray(), kp2, desc2);

        if (desc1.empty() || desc2.empty()) {
            rel_offsets.push_back(cv::Point2d(0, 0));
            continue;
        }

        std::vector<std::vector<cv::DMatch>> matches;
        bf.knnMatch(desc1, desc2, matches, 2);

        std::vector<double> dxs, dys;
        for (auto& m : matches) {
            if (m.size() == 2 && m[0].distance < ratio_ * m[1].distance) {
                cv::Point2d p1 = kp1[m[0].queryIdx].pt;
                cv::Point2d p2 = kp2[m[0].trainIdx].pt;
                dxs.push_back(p1.x - p2.x);
                dys.push_back(p1.y - p2.y);
            }
        }

        if (dxs.size() < min_matches_) {
            rel_offsets.push_back(cv::Point2d(0, 0));
            continue;
        }

        std::sort(dxs.begin(), dxs.end());
        std::sort(dys.begin(), dys.end());
        
        // 计算 dx 和 dy 的中位数
        double mdx = dxs[dxs.size() / 2];
        double mdy = dys[dys.size() / 2];
        rel_offsets.push_back(cv::Point2d(mdx, mdy));
        
        printf("图像 %zu -> %zu 偏移: dx=%.2f, dy=%.2f\n", i, i+1, mdx, mdy);
    }
    return true;
}