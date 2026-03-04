cmake_minimum_required(VERSION 3.10)
project(opencv_lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# --- 核心配置：注意改成你所安装的opencv的路径 ---
set(OPENCV_BASE "D:/opencv/build")

# 1. 头文件路径
set(OPENCV_INCLUDE_DIR "${OPENCV_BASE}/include")

# 2. 库文件目录 
set(OPENCV_LIB_DIR "${OPENCV_BASE}/x64/vc15/lib")

# 3. 具体的库文件名
set(OPENCV_LIB_NAME "opencv_world345d.lib") 

# --- 开始构建 ---

include_directories(${OPENCV_INCLUDE_DIR})

add_executable(opencv_lab1 main.cpp)

# 链接库文件
target_link_directories(opencv_lab1 PRIVATE ${OPENCV_LIB_DIR})
target_link_libraries(opencv_lab1 ${OPENCV_LIB_NAME})

# 4. 自动拷贝 DLL：注意这里也要指向 vc15 文件夹
add_custom_command(TARGET opencv_lab1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${OPENCV_BASE}/x64/vc15/bin/opencv_world345d.dll" 
    $<TARGET_FILE_DIR:opencv_lab1>)