#include <iostream>
#include <typeinfo>
class Base
{
    int a{0};
    int b{20};
    double c{0};
};

int main()
{
    // 1. 基本类型
    int x = 42;
    double y = 3.14;
    Base z;

    std::cout << typeid(x).name() << std::endl; // 输出类似 "int"
    std::cout << typeid(y).name() << std::endl; // 输出类似 "double"
    std::cout << typeid(z).name() << std::endl;

    // 2. Lambda 类型
    auto lambda1 = [](int a)
    { return a; };
    auto lambda2 = [](int a)
    { return a * 2; };

    std::cout << typeid(lambda1).name() << std::endl; // 输出类似 "lambda..."
    std::cout << typeid(lambda2).name() << std::endl; // 输出不同的名字

    // 3. 比较是否相同
    std::cout << (typeid(lambda1) == typeid(lambda2)) << std::endl; // 0（false）

    // 4. 函数指针
    int (*fp)(int) = nullptr;
    std::cout << typeid(fp).name() << std::endl; // 输出类似 "int (*)(int)"

    // 5. 仿函数
    std::plus<int> plus_obj;
    std::cout << typeid(plus_obj).name() << std::endl; // 输出类似 "class std::plus..."

    return 0;
}