#pragma once
#include "hw7_pa.h"
#include "ImageBlender.hpp"
#include "Interpolator.hpp"
#include "OffsetEstimator.hpp"
#include <opencv2/core/mat.hpp>
#include <memory>
#include <vector>

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