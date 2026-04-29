#ifndef HW8_PA_H
#define HW8_PA_H

#include "opencv2/core/mat.hpp"
#include <vector>
class CylindricalPanorama {
public:
    virtual bool makePanorama(
        std::vector<cv::Mat>& img_vec, cv::Mat& img_out, double f) = 0;
};

#endif
