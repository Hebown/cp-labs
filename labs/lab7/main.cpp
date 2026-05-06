#include "ImageSet.hpp"
#include "Interpolator.hpp"
#include "Panorama.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

namespace fs = std::filesystem;

void printUsage(const char* prog) {
    std::cout << "用法: " << prog << " <根目录> <输出目录> [混合器类型] [插值器类型] [偏移估计器类型]\n"
              << "  混合器类型: linear (默认)\n"
              << "  插值器类型: nearest 或 bilinear (默认)\n"
              << "  偏移估计器类型: orb (默认)\n";
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    fs::path root(argv[1]);
    fs::path outRoot(argv[2]);
    if (!fs::exists(root) || !fs::is_directory(root)) {
        std::cerr << "错误: 根目录不存在或不是目录\n";
        return 1;
    }
    fs::create_directories(outRoot);

    // 混合器类型
    std::string blenderType = (argc >= 4) ? argv[3] : "linear";
    std::unique_ptr<ImageBlender> blender;
    if (blenderType == "linear") {
        blender = std::make_unique<LinearBlender>();
        std::cout << "使用线性混合器\n";
    } else {
        std::cerr << "不支持的混合器类型: " << blenderType << "\n";
        return 1;
    }

    // 插值器类型
    std::string interpType = (argc >= 5) ? argv[4] : "bilinear";
    std::unique_ptr<Interpolator> interpolator;
    if (interpType == "nearest") {
        interpolator = std::make_unique<NearestInterpolator>();
        std::cout << "使用最近邻插值\n";
    } else if (interpType == "bilinear") {
        interpolator = std::make_unique<BilinearInterpolator>();
        std::cout << "使用双线性插值\n";
    } else if (interpType == "bicubic") {
        interpolator = std::make_unique<BicubicInterpolator>();
        std::cout << "使用双三次插值\n";
    } else {
        std::cerr << "不支持的插值器类型: " << interpType << "\n";
        return 1;
    }

    std::string offsetType   = (argc >= 6) ? argv[5] : "orb";
    std::unique_ptr<OffsetEstimator> offsetEstimator;
    if (offsetType == "orb") {
        offsetEstimator = std::make_unique<OrbMedianOffsetEstimator>();
        std::cout << "使用 ORB + 中位数偏移估计\n";
    } else {
        std::cerr << "不支持的偏移估计器: " << offsetType << "\n";
        return 1;
    }

    Panorama pano(std::move(blender), std::move(interpolator), std::move(offsetEstimator));

    // 遍历根目录下的所有子文件夹
    for (const auto& entry : fs::directory_iterator(root)) {
        if (!entry.is_directory()) continue;

        fs::path subdir = entry.path();
        std::cout << "\n处理文件夹: " << subdir << std::endl;

        ImageSet imgSet(subdir.string());
        if (!imgSet.load()) {
            std::cerr << "加载失败，跳过\n";
            continue;
        }

        const auto& images = imgSet.getImages();
        if (images.empty()) {
            std::cerr << "没有有效图片，跳过\n";
            continue;
        }

        double f = imgSet.getCameraConfig().focus;
        std::cout << "焦距: " << f << ", 图片数量: " << images.size() << std::endl;

        std::vector<cv::Mat> imgVec = images;
        cv::Mat panorama;
        if (pano.makePanorama(imgVec, panorama, f)) {
            fs::path outPath = outRoot / (subdir.filename().string() + "_panorama.jpg");
            cv::imwrite(outPath.string(), panorama);
            std::cout << "全景图已保存: " << outPath << std::endl;
        } else {
            std::cerr << "全景图生成失败\n";
        }
    }

    return 0;
}