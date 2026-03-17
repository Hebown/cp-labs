#include <opencv2/opencv.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

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
            BoxFilter filter(width,height);
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