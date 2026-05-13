#include <iostream>
#include <string>
#include "e:\Projects\mySTL\includes\allocator\_function.hpp"

// Test 1: Regular function
int add(int a, int b)
{
    return a + b;
}

// Test 2: Functor
struct Multiplier
{
    int factor;
    Multiplier(int f) : factor(f) {}
    int operator()(int x) const
    {
        return x * factor;
    }
};

// Test 3: Lambda with capture
void test_lambda_with_capture()
{
    std::string msg = "Hello";
    MySTL::Function<void()> f = [&msg]()
    {
        std::cout << msg << " World!" << std::endl;
    };
    f();
}

int main()
{
    std::cout << "=== Function Tests ===" << std::endl;

    // Test 1: Store regular function
    std::cout << "\n1. Regular Function Test" << std::endl;
    MySTL::Function<int(int, int)> f1 = add;
    std::cout << "add(3, 5) = " << f1(3, 5) << std::endl;

    // Test 2: Store lambda
    std::cout << "\n2. Lambda Test" << std::endl;
    MySTL::Function<int(int)> f2 = [](int x)
    { return x * 2; };
    std::cout << "lambda(4) = " << f2(4) << std::endl;

    // Test 3: Store functor
    std::cout << "\n3. Functor Test" << std::endl;
    MySTL::Function<int(int)> f3 = Multiplier{3};
    std::cout << "Multiplier(3)(5) = " << f3(5) << std::endl;

    // Test 4: Copy constructor
    std::cout << "\n4. Copy Constructor Test" << std::endl;
    MySTL::Function<int(int)> f4 = f2;
    std::cout << "f4(5) = " << f4(5) << std::endl;

    // Test 5: Move constructor
    std::cout << "\n5. Move Constructor Test" << std::endl;
    MySTL::Function<int(int)> f5 = std::move(f4);
    std::cout << "f5(6) = " << f5(6) << std::endl;

    // Test 6: Empty function handling
    std::cout << "\n6. Empty Function Test" << std::endl;
    MySTL::Function<int(int)> f6;
    std::cout << "f6 is empty: " << std::boolalpha << !f6 << std::endl;

    // Test 7: Type query
    std::cout << "\n7. Type Query Test" << std::endl;
    MySTL::Function<int(int)> f7 = [](int x)
    { return x + 1; };
    std::cout << "f7 target_type: " << f7.target_type().name() << std::endl;

    // Test 8: Lambda with capture
    std::cout << "\n8. Lambda with Capture Test" << std::endl;
    test_lambda_with_capture();

    // Test 9: bool conversion
    std::cout << "\n9. Bool Conversion Test" << std::endl;
    MySTL::Function<void()> f8 = []()
    { std::cout << "Hello!" << std::endl; };
    if (f8)
    {
        f8();
    }

    std::cout << "\n=== All Tests Passed ===" << std::endl;
    return 0;
}