#include <numeric>
#include <opencv2/opencv.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
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
class BoxFilter{
    public:
    BoxFilter(int width,int height){
        int matrixWidth=2*width+1;
        int matrixHeight=2*height+1;
        boxFilter=cv::Mat(matrixHeight,matrixWidth,CV_64F,cv::Scalar(1.0/matrixHeight/matrixWidth));
    }
    cv::Mat filter(const cv::Mat& image){
        cv::Mat result;
        cv::filter2D(image, result, -1, boxFilter);
        return result;
    }
    private:
    cv::Mat boxFilter;
};
class Solution{
    public:
        void run(const std::string&inputFilePath,const std::string& outputFilePath,int width,int height){
            Filter filter(width,height,[=](int i,int j){
                return 1/(width*2+1)/(height*2+1);
            });
            cv::Mat image=cv::imread(inputFilePath,cv::IMREAD_COLOR);
            cv::Mat result=filter.filter(image);
            cv::imwrite(outputFilePath, result);
        }
    private:
};


int main(int argc, char** argv){
    try {
        if(argc!=5){
            throw std::runtime_error("参数数量错误，期望获得四个参数");
        }
        std::string inputFilePath=argv[1],outputFilePath=argv[2];
        int width=std::stoi(argv[3]);
        int height=std::stoi(argv[4]);
        Solution solution;
        solution.run(inputFilePath,outputFilePath,width,height);
    } catch (std::exception& e) {
        std::cerr<<"[ERROR]: "<<e.what()<<std::endl;
    }
}