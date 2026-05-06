#include "ImageSet.hpp"
#include "opencv2/imgcodecs.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <algorithm>

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