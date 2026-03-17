#include "utils.hpp"
#include <cstddef>
#include <numbers>
#include <exception>
#include <string>


int main(int argc,char**argv){
    try{
        std::string inputFilePath=argv[1],outputFilePath=argv[2];
        double sigma=std::stod(argv[3]);
        size_t size=floor(5*sigma);
        Solution solution;
        solution.run(inputFilePath, outputFilePath, size, size, [=](size_t i,size_t j){
            int ix=static_cast<int>(size)-static_cast<int>(i),iy=static_cast<int>(size)-static_cast<int>(j);
            return 1.0/(2*std::numbers::pi*sigma*sigma) * exp(-(ix*ix+iy*iy)/(2*sigma*sigma));
        });
    }catch(std::exception& e){
        std::cerr<<"[ERROR]: "<<e.what()<<std::endl;
    }
}