#pragma once
#include "hw7_pa.h"
#include "opencv2/core/mat.hpp"
#include <memory>
#include <string>
#include <vector>

struct CameraConfig {
    double focus;
};

class ImageSet {
private:
    std::string setPath;
    CameraConfig cameraConfig;
    bool is_loaded;
    std::vector<cv::Mat> images;
public:
    ImageSet(std::string rootPath);
    bool load();
    const std::vector<cv::Mat>& getImages() const;
    const CameraConfig& getCameraConfig() const;
    bool isLoaded() const;
    void unload();
};

class Interpolator {
public:
    virtual ~Interpolator() = default;
    // 从单通道或三通道图像中采样 (x, y) 处的像素值，坐标可为浮点
    virtual cv::Vec3b sample(const cv::Mat& image, double x, double y) const = 0;
};

class NearestInterpolator : public Interpolator {
public:
    cv::Vec3b sample(const cv::Mat& image, double x, double y) const override;
};

class BilinearInterpolator : public Interpolator {
public:
    cv::Vec3b sample(const cv::Mat& image, double x, double y) const override;
};

class CylindricalProjector{
    public:
    // 柱面坐标 -> 原始图像坐标
    static cv::Point2d backward(double xp, double yp, double f, double cx, double cy);

    // 将原始图像投影到柱面，生成柱面图像
    // src: 原始图像
    // f: 焦距（像素）
    // dst: 输出柱面图像
    static void project(const cv::Mat& src, double f, cv::Mat& dst, const Interpolator& interpolator);
};

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

class ImageBlender {
public:
    virtual ~ImageBlender() = default;
    // 输入：柱面图像列表，每张图像在最终画布中的左上角x坐标（y均为0），画布尺寸（预先计算好）
    // 输出：融合后的全景图
    virtual void blend(const std::vector<cv::Mat>& images,
                       const std::vector<int>& x_offsets,
                       const cv::Size& canvas_size,
                       cv::Mat& result) = 0;
};

class LinearBlender : public ImageBlender {
public:
    void blend(const std::vector<cv::Mat>& images,
               const std::vector<int>& x_offsets,
               const cv::Size& canvas_size,
               cv::Mat& result) override;
};

class Panorama : public CylindricalPanorama {
private:
    std::unique_ptr<ImageBlender> imageBlenderPtr;
    std::unique_ptr<Interpolator> interpolatorPtr;
    std::unique_ptr<OffsetEstimator> offsetEstimatorPtr;
public:
    explicit Panorama(
        std::unique_ptr<ImageBlender> imageBlenderPtr,
        std::unique_ptr<Interpolator> interpolatorPtr,
        std::unique_ptr<OffsetEstimator> offsetEstimatorPtr
    )
        : imageBlenderPtr(std::move(imageBlenderPtr)),
          interpolatorPtr(std::move(interpolatorPtr)),
          offsetEstimatorPtr(std::move(offsetEstimatorPtr)) {}

    virtual bool makePanorama(std::vector<cv::Mat>& img_vec, cv::Mat& img_out, double f) override;
};