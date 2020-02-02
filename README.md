# CPPExperiment
C++的一些实验项目(主要在Windows环境中)

## CMakeLists学习
C++ CMakeLists的Hello World版本，[参考代码](learn-cmake/HelloWorld/CMakeLists.txt)

需要安装以下软件：
- mingw-w64
- CMake

注意事项：
> 安装了mingw-w64，把bin下的mingw32-make.exe复制一份并改名为make.exe。



Ctrl+Shift+P 输入下面的命令为CMake指定编译工具链
> CMake:Select a Kit

运行的命令如下：
- cd build
- make
- 直接输入**.exe文件即可

[参考1](https://blog.csdn.net/qq_39942341/article/details/98964620)

[参考2](https://blog.csdn.net/MakerCloud/article/details/93545513)

## Windows使用C++调用Pytorch1.0模型

[参考代码](example-app/CMakeLists.txt)

注意：
Libtorch要下载稳定版，不要下载Nightly版本的。否则会发生"找不到Torch所在路径"的错误。


参考：
[pyTorch官网的Minimal Example](https://pytorch.org/cppdocs/installing.html)
[Windows使用C++调用Pytorch1.0模型](https://zhuanlan.zhihu.com/p/52806730)
