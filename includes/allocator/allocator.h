#pragma once
#ifndef MYSTL
#define MYSTL
#include <vector>
// 分配器
namespace SWDSTL
{
    template <class T>
    class allocator
    {
        using value_type = T;
        using pointer = T *using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = suze_t;
        using differnce_type = ptrdiff_t;
    };
}
#endif