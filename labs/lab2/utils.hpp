#pragma once
#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"
#include <functional>
#include <vector>
#include <opencv2/opencv.hpp>
#include <execution>
class FilterBase{
    public:
    virtual cv::Mat filter(const cv::Mat& image)=0;
};
class ValueFilter:public FilterBase{ // 这种Filter内部固定值，比如均值、高斯，与图像无关
    public:
    ValueFilter(
        int kernel_w,
        int kernel_h,
        std::function<double(size_t,size_t)> value_function
    )
    : kernel(kernel_h*2+1, kernel_w*2+1, CV_64F)
    {
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
class RuleFilter:public FilterBase{ // 这种Filter没有内部固定值，根据规则和图像的具体值来修改
    public:
    RuleFilter(
        int width,
        int height,
        std::function<cv::Vec3b(const cv::Mat&,int, int)>rule
    )
    : width(width),
      height(height),
      rule(rule){}

    cv::Mat filter(const cv::Mat& image)override{
        cv::Mat result=image.clone();
        for(int i=height;i<image.rows-height;i++){
            for(int j=width;j<image.cols-width;j++){
                result.at<cv::Vec3b>(i,j)=rule(image,i,j);
            }
        }return result;
    }
    private:
    int width,height;
    std::function<cv::Vec3b(const cv::Mat&,int i,int j)>rule;
};

class Solution{ // 这里负责处理每个程序的输入
    public:
        void run(const std::string& inputFilePath,
                 const std::string& outputFilePath, 
                 int width, 
                 int height,
                 std::function<double(size_t,size_t)>value_function)
        {
            ValueFilter filter(width,height,value_function);
            cv::Mat image=cv::imread(inputFilePath,cv::IMREAD_COLOR);
            cv::Mat result=filter.filter(image);
            cv::imwrite(outputFilePath, result);
        }
        void run(const std::string& inputFilePath,
                 const std::string& outputFilePath, 
                 int width, 
                 int height,
                 std::function<cv::Vec3b(const cv::Mat&,int,int)>rules_function)
        {
            RuleFilter filter(width,height,rules_function);
            cv::Mat image=cv::imread(inputFilePath,cv::IMREAD_COLOR);
            cv::Mat result=filter.filter(image);
            cv::imwrite(outputFilePath, result);
        }
};