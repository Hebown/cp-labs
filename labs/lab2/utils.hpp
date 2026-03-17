#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <execution>
class Filter{
    public:
    Filter(int kernel_w,int kernel_h,std::function<double(size_t,size_t)> value_function):kernel(kernel_h*2+1, kernel_w*2+1, CV_64F){
        std::vector<int>row_indices(2*kernel_h+1);
        std::iota(row_indices.begin(), row_indices.end(), 0);
        std::for_each(std::execution::par,row_indices.begin(),row_indices.end(),
            [&](int r){
                double* row_ptr=kernel.ptr<double>(r);
                for(int c=0;c<2*kernel_w+1;c++){
                    row_ptr[c]=value_function(r,c);
                }
            }
        );
    }
    cv::Mat filter(const cv::Mat& image){
        cv::Mat result;
        cv::filter2D(image, result, -1, kernel);
        return result;
    }
    private:
    cv::Mat kernel;
};

class Solution{ // 这里负责处理每个程序的输入
    public:
        void run(const std::string& inputFilePath,
                 const std::string& outputFilePath, 
                 int width, 
                 int height,
                 std::function<double(size_t,size_t)>value_function)
        {
            Filter filter(width,height,value_function);
            cv::Mat image=cv::imread(inputFilePath,cv::IMREAD_COLOR);
            cv::Mat result=filter.filter(image);
            cv::imwrite(outputFilePath, result);
        }
};