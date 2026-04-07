[实验文档中心](http://www.cad.zju.edu.cn/home/gfzhang/course/computational-photography)

- [计算摄影学实验仓库](#计算摄影学实验仓库)
  - [lab2 须知](#lab2-须知)
    - [构建环境](#构建环境)
    - [项目信息](#项目信息)
  - [lab3 须知](#lab3-须知)


# 计算摄影学实验仓库

## lab2 须知
### 构建环境
我的开发环境是 Windows 11 x64 系统 + Visual Studio 2026 的独立 MSVC 工具链（无完整 IDE 安装）+ OpenCV 4.12.0 。构建出来的程序是64位程序。

因为我并未安装完整的 visual studio 实例，只有独立的 MSVC 工具链，所以在 configure/generate 我的项目之前，你需要找到 MSVC 工具链必要的配置环境的 bat 脚本，并在一个 cmd 终端中运行，以告诉 cmake 这些工具都在什么位置。

这个脚本一般位于 `你的 VS 安装路径\VC\Auxiliary\Build` 下，比如在我的电脑上，这个路径是 `d:\VS2026\VC\Auxiliary\Build\vcvars64.bat`。因为我的机子是 64 位的（应该都是这个吧，要是你的机子是 32 位的运行 32 位的环境配置脚本即可）

运行这个脚本后你应当看到类似以下输出
```txt
**********************************************************************
** Visual Studio 2026 Developer Command Prompt v18.2.1
** Copyright (c) 2025 Microsoft Corporation
**********************************************************************
[DEBUG:ext\vcvars.bat] Found potential v145 version file: 'Microsoft.VCToolsVersion.VC.14.50.18.0.txt'
[DEBUG:ext\vcvars.bat] Testing v145 version file: 'Microsoft.VCToolsVersion.VC.14.50.18.0.txt'
[vcvarsall.bat] Environment initialized for: 'x64'
```

然后在根目录下运行以下脚本即可
```
cmake -B build
cmake --build build
```
你就可以在`build\labs\lab2`中得到所有的二进制文件了。

为了方便你运行，我提供了一些demo给你，这些 demo 要用的图片我放到了 `demo\assets\labs\lab2\demo-picture` 下，你可以在 demo 子目录中利用一个这样的脚本

```
.\build\labs\lab2\BoxFilter.exe assets\labs\lab2\demo-pictures\opencv-logo.png assets\labs\lab2\result-pictures\box-result-logo.png 5 5
```
来观察结果

> 当你需要测试双边滤波的时候，可能得换到另一张图

### 项目信息
- report：文件夹，存放报告
- labs：存储着 lab 源代码，
    > 如果你愿意，可以到我的[仓库](https://github.com/Hebown/cp-labs)去看看，你可以获得更好的项目重建体验
    > 如果你访问失败了，那是因为我还没上传...
- assets：图片文件夹
- CMakeLists.txt：所有labs的构建文本文件
- demo：你运行测试的地方，`demo\build` 下我已放好了二进制文件

依旧 orz

## lab3 须知
lab3 的依赖非常简单，所以它的内容也很少，其大体结构和 [lab2](#lab2-须知) 类似，这里我主要说几点改动，可能对测试会有影响

1. 我创建了Sparse的构造函数，构造时必须指定Sparse的长和宽，因此在我的 main 代码中，你会看到：
   ```cpp
    int main() {
        /* 测试稀疏矩阵 */
        Sparse S(6,6); // 这里必须指定稀疏矩阵的长和宽
        // ...
    }
   ```
   因此如果未编译通过，请考虑这个问题。
2. 我不再使用 Makefile 作为我的构建脚本，因此构建方法论应该与之前的 [构建方式](#构建环境) 一致，具体脚本是在根目录下
   ```cmd
    cmake -B build
    cmake --build build
    .\build\opencv_lab3_main.exe
   ``` 
    即可观察到结果
3. demo 文件夹中我放了一个二进制文件，它应当可以直接运行

依旧 orz.