#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "opencv2/core/hal/interface.h"
#include "utils.hpp"

int main(int argc, char** argv){
    try {
        if(argc!=5){
            throw std::runtime_error("参数数量错误，期望获得四个参数");
        }
        std::string inputFilePath=argv[1],outputFilePath=argv[2];
        int width=std::stoi(argv[3]);
        int height=std::stoi(argv[4]);
        Solution solution;
        solution.run(inputFilePath,outputFilePath,width,height,[=](const cv::Mat&image,int i,int j){
            std::vector<uchar>values[3];
            for(int iy=-height;iy<=height;iy++){
                for(int ix=-width;ix<=width;ix++){
                    int ni=i+iy;
                    int nj=j+ix;

                    for(int c=0;c<image.channels();c++){
                        values[c].push_back(image.at<cv::Vec3b>(ni,nj)[c]);
                    }
                }
            }
            cv::Vec3b result;
            for(int c=0;c<image.channels();c++){
                auto& vec=values[c];
                std::nth_element(vec.begin(),vec.begin()+vec.size()/2,vec.end());
                result[c]=vec[vec.size()/2];
            }
            return result;
        });
    } catch (std::exception& e) {
        std::cerr<<"[ERROR]: "<<e.what()<<std::endl;
    }
}