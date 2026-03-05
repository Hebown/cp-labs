cmake_minimum_required(VERSION 3.10)
project(opencv_lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8") # 修改编译参数形式
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 核心配置：注意改成你所安装的opencv的路径 ---
set(OPENCV_BASE "${CMAKE_SOURCE_DIR}/opencv_gnu")

# 1. 头文件路径
set(OPENCV_INCLUDE_DIR "${OPENCV_BASE}/include")

# 2. 库文件目录 
set(OPENCV_LIB_DIR "${OPENCV_BASE}/lib") # 导入库
set(OPENCV_DLL_DIR "${OPENCV_BASE}/bin") # 动态库

# 3. 具体的库文件名
set(OPENCV_LIBRARIES
    "libopencv_core4130"
    "libopencv_imgproc4130"
    "libopencv_imgcodecs4130"
    "libopencv_highgui4130"
)
# --- 开始构建 ---

include_directories(${OPENCV_INCLUDE_DIR})

add_executable(opencv_lab1 main.cpp)

# 链接库文件
target_link_directories(opencv_lab1 PRIVATE ${OPENCV_LIB_DIR})
foreach(lib_name ${OPENCV_LIBRARIES})
    set(lib_path "${OPENCV_LIB_DIR}/${lib_name}.dll.a")
    set(dll_path "${OPENCV_DLL_DIR}/${lib_name}.dll")
    target_link_libraries(opencv_lab1 PRIVATE ${lib_path})
    add_custom_command(TARGET opencv_lab1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dll_path} $<TARGET_FILE_DIR:opencv_lab1>
    )
endforeach()

# # 4. 自动拷贝 DLL：注意这里也要指向 vc15 文件夹
# add_custom_command(TARGET opencv_lab1 POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${OPENCV_BASE}/x64/vc15/bin/opencv_world345d.dll" 
#     $<TARGET_FILE_DIR:opencv_lab1>)