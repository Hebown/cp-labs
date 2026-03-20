project(opencv_lab2)

add_project(BoxFilter BoxFilter.cpp)

set(CMAKE_CXX_STANDARD 20)

add_project(GaussianFilter GaussianFilter.cpp)

set(CMAKE_CXX_STANDARD 17)


add_project(MedianFilter MedianFilter.cpp)
add_project(BilateralFilter BilateralFilter.cpp)
add_project(Fourier Fourier.cpp)
