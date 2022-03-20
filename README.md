# STLite
Learning STL according to STL源码剖析

-------

## 介绍

根据《STL源码剖析》，实现了基础的二级配置器及数据结构，相比于工业使用的STL来说，没有过多的宏定义，更方便阅读学习。代码使用c++11，通过cmake管理代码，增加了gtest单测，方便学习和测试代码。

## 代码结构

项目整体结构如下，src为源码目录，tests存放测试文件。

```shell

STLite
├── CMakeLists.txt          // 项目文件
├── main.cpp
├── README.md
├── simplealloc.hpp         // 简单的空间配置器
├── src                     // 源码
│   └── stlite
│       ├── jw_algorithm.hpp
│       ├── jw_alloc.hpp
│       ├── ......
│
└── tests                   // 测试
    ├── CMakeLists.txt      // 子目录项目文件
    ├── jw_list_test.cpp
    └── jw_vector_test.cpp

```

## 环境说明

推荐使用vscode作为编辑器，安装上扩展为:c/c++、cmake、cmake tools，能够极大的方便编写效率。

需要安装的工具，以ubuntu为例：

```shell

sudo apt install cmake ninja-build # cmake工具
sudo apt install libgtest-dev      # gtest
sudo apt install clang             # 编译器，可以结合clangd使用

```

## TODO

- [ ] 增加高级数据结构，如线段树等；
- [ ] 增加智能指针、协程等工具类；
- [ ] ......
