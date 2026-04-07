project(opencv_lab3 CXX)

set(CMAKE_CXX_STANDARD 17)

# 本实现不需要 opencv 库

add_executable(
    opencv_lab3_main
    hw3_solve.cpp
    main.cpp
    sparse.cpp
)

target_compile_options( opencv_lab3_main PRIVATE
    /O2
)
