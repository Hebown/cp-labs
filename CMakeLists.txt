cmake_minimum_required(VERSION 3.30)

project(cp-labs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# --- 核心配置：注意改成你所安装的opencv的路径 ---
set(OPENCV_BASE "D:/OpenCV/opencv/build")

# 1. 头文件路径
set(OPENCV_INCLUDE_DIR "${OPENCV_BASE}/include")

# 2. 库文件目录 
set(OPENCV_LIB_DIR "${OPENCV_BASE}/x64/vc16/lib")

# 3. 具体的库文件名
set(OPENCV_LIB_NAME "opencv_world4120.lib")

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)

# 辅助函数
function(add_project target)
    add_executable(${target} ${ARGN})
    # 链接库文件
    target_link_directories(${target} PRIVATE ${OPENCV_LIB_DIR})
    target_link_libraries(${target} ${OPENCV_LIB_NAME})

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENCV_BASE}/x64/vc16/bin/opencv_world4120.dll" 
        $<TARGET_FILE_DIR:${target}>)
    
endfunction()

include_directories(${OPENCV_INCLUDE_DIR}) # 添加头文件

# 关闭 showIncludes
string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_C "${CMAKE_DEPFILE_FLAGS_C}")
string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_CXX "${CMAKE_DEPFILE_FLAGS_CXX}")

add_subdirectory(labs/lab1)
add_subdirectory(labs/lab2)
add_subdirectory(labs/lab3)
add_subdirectory(labs/lab5)
add_subdirectory(labs/tasks)
add_subdirectory(labs/lab7)
