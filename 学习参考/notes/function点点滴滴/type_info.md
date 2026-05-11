# type_info
## 简介
之前学习了type_traits 也就是类型特征，主要是用于编译时期的类型检查
但是这个type_info是运行时类型信息
type_traits是新生代出生于C++11,而type_info C++98就有了

## type_info 相关说明
### typeid运算符
typeid跟sizeof,alignof一样都是运算符，typeid(x)用于获取x的类型信息然后返回一个type_info类型的引用。
为什么是引用呢？因为这些类型信息多个变量共享的（值类型就会拷贝）
那自定义变量的类型信息怎么来的？由编译器生成的

# 其他资料
[测试实验程序](testsrc/test2.cpp)