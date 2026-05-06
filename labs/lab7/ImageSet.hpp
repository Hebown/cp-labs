#pragma once
#include "CameraConfig.hpp"
#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>

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