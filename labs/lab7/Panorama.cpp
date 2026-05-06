#include "Panorama.hpp"
#include <memory>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include <random>

ImageSet::ImageSet(std::string rootPath)
    : setPath(std::move(rootPath)), is_loaded(false) {
    printf("初始化图片集 %s\n", setPath.c_str());
}

bool ImageSet::load() {
    if (is_loaded) {
        printf("此图片集 %s 已 load\n", setPath.c_str());
        return true;
    }
    printf("loading 图片集 %s\n", setPath.c_str());

    namespace fs = std::filesystem;
    fs::path folder(setPath);

    // 读取 K.txt
    printf("正在加载焦距\n");
    auto kPath = folder / "K.txt";
    std::ifstream kfile(kPath);
    if (!kfile.is_open()) {
        printf("无法打开 %s\n", kPath.generic_string().c_str());
        return false;
    }
    kfile >> cameraConfig.focus;

    // 收集所有 .jpg 文件
    std::vector<fs::path> jpgs;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".jpg" || ext == ".jpeg") {
                jpgs.push_back(entry.path());
            }
        }
    }
    std::sort(jpgs.begin(), jpgs.end());

    // 加载图片
    images.clear();
    images.reserve(jpgs.size());
    for (const auto& p : jpgs) {
        cv::Mat img = cv::imread(p.string(), cv::IMREAD_COLOR);
        if (img.empty()) {
            printf("警告: 无法读取图片 %s\n", p.generic_string().c_str());
            continue;
        }
        images.push_back(std::move(img));
    }

    is_loaded = true;
    printf("加载完成，共 %zu 张图片\n", images.size());
    return true;
}

const std::vector<cv::Mat>& ImageSet::getImages() const {
    return images;
}

const CameraConfig& ImageSet::getCameraConfig() const {
    return cameraConfig;
}

bool ImageSet::isLoaded() const {
    return is_loaded;
}

void ImageSet::unload() {
    images.clear();
    images.shrink_to_fit();
    is_loaded = false;
    printf("已卸载图片集 %s\n", setPath.c_str());
}

cv::Vec3b NearestInterpolator::sample(const cv::Mat& image, double x, double y) const {
    int ix = static_cast<int>(std::round(x));
    int iy = static_cast<int>(std::round(y));
    if (ix < 0 || ix >= image.cols || iy < 0 || iy >= image.rows)
        return cv::Vec3b(0, 0, 0);
    return image.at<cv::Vec3b>(iy, ix);
}

cv::Vec3b BilinearInterpolator::sample(const cv::Mat& image, double x, double y) const {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x0 < 0 || x1 >= image.cols || y0 < 0 || y1 >= image.rows)
        return cv::Vec3b(0, 0, 0);

    double dx = x - x0;
    double dy = y - y0;

    cv::Vec3b v00 = image.at<cv::Vec3b>(y0, x0);
    cv::Vec3b v10 = image.at<cv::Vec3b>(y0, x1);
    cv::Vec3b v01 = image.at<cv::Vec3b>(y1, x0);
    cv::Vec3b v11 = image.at<cv::Vec3b>(y1, x1);

    cv::Vec3b result;
    for (int c = 0; c < 3; ++c) {
        double top = (1 - dx) * v00[c] + dx * v10[c];
        double bottom = (1 - dx) * v01[c] + dx * v11[c];
        double val = (1 - dy) * top + dy * bottom;
        result[c] = static_cast<uchar>(val);
    }
    return result;
}

cv::Point2d CylindricalProjector::backward(double xp, double yp, double f, double cx, double cy) {
    double xp_centered = xp - cx;
    double yp_centered = yp - cy;

    double theta = xp_centered / f;
    double x_centered = f * tan(theta);
    double y_centered = yp_centered * sqrt(x_centered * x_centered + f * f) / f;

    return cv::Point2d(x_centered + cx, y_centered + cy);
}

void CylindricalProjector::project(const cv::Mat& src, double f, cv::Mat& dst,const Interpolator& interpolator) {
    CV_Assert(!src.empty() && f > 0);
    int src_w = src.cols, src_h = src.rows;
    double cx = (src_w - 1) / 2.0;
    double cy = (src_h - 1) / 2.0;

    // 计算柱面图像尺寸
    double hfov_rad = 2.0 * atan(src_w / (2.0 * f));
    int dst_w = static_cast<int>(f * hfov_rad);
    int dst_h = src_h;

    // 反向映射：遍历柱面每个像素，找到原始图像对应位置并插值
    dst = cv::Mat::zeros(dst_h, dst_w, src.type());

    for (int xp = 0; xp < dst_w; ++xp) {
        for (int yp = 0; yp < dst_h; ++yp) {
            cv::Point2d orig = backward(xp, yp, f, cx, cy);
            dst.at<cv::Vec3b>(yp, xp) = interpolator.sample(src, orig.x, orig.y);
        }
    }
}


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