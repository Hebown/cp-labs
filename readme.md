克隆仓库方法

```
git clone https://github.com/Hebown/cp-labs
```

# 计算摄影学 labs
此项目基于 Windows MSVC 工具链 + CMake + OpenCV 4.12。

我并未使用 VS 搭建工程，因此你需要按照如下流程编译：
1. 打开一个位于项目根目录的终端（建议cmd）
2. 使用 MSVC 工具链中的 "\VC\Auxiliary\Build\vcvars64.bat" 配置环境变量。例如我的 脚本文件位于 VS2026 工具包下，完全的路径为 "D:\VS2026\VC\Auxiliary\Build\vcvars64.bat"
3. 确保使用正确的 OpenCV 库文件，我的库是 4120版本的，所以这里需要修改；同时还要修改opencv所在位置。
4. 确保配置无误后，运行以下脚本
    ```cmd
    cmake -B build
    cmake --build build
    ```
5. 添加基本图片 opencv-logo 到build文件夹下，并 cd build，即可开始测试
6. 如果你想检查不同的功能，可以查阅我之前的 commit（关键在 task-list 文件中）。按照任务先后，这些commit的顺序依次为
   ```
    68cc989 完成了图像访问和反色
    73a3f63 完成了Scalar反色
    40aa99c 完成了求逆和显色
   ```