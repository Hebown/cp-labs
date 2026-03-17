#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
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
        solution.run(inputFilePath,outputFilePath,width,height,[=](int i,int j){
            return 1.0/(width*2+1)/(height*2+1); // MeanFilter 的核心逻辑
        });
    } catch (std::exception& e) {
        std::cerr<<"[ERROR]: "<<e.what()<<std::endl;
    }
}