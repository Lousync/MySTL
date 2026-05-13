#pragma once 

#include<utility>
#include<type_traits>

/*智能指针设计需要知道的一些点
1. 智能指针不能拷贝智能移动
2. 不同类型的析构会有特定的方法需要特例化
*/

/*
所谓智能指针也就是对普通指针的包装，在业务上没什么特殊点
主要原理是利用了C++的RAII机制，在变量离开其作用域时自动调用析构函数来释放资源
*/
template<typename _Tp>
struct Default_Deleter{
    void operator()(_Tp* ptr) const{
        delete ptr;
    }
};

//偏特化用于实现对于数组的析构
template<typename _Tp>
struct Default_Deleter<_Tp[]>{
    void operator()(_Tp* ptr) const{
        delete[] ptr;
    }
};


template<typename T,typename Deleter = Default_Deleter<T>>
class Unique_ptr{
public:
    using element_type = T;
    using deleter_type = Deleter;
    using pointer = T*;

    //------------------------构造函数和析构函数------------------------------
    Unique_ptr(const Unique_ptr&) = delete;
    Unique_ptr& operator=(const Unique_ptr&) = delete;

    Unique_ptr(std::nullptr_t=nullptr)noexcept: M_Ptr(nullptr){}

    /*这个构造函数允许 栈变量取地址而造成的隐式转换*/
    explicit Unique_ptr(T* ptr)noexcept: M_Ptr(ptr){}

    //移动构造函数,支持多态的跨类型转化
    template<class _Up,class _UDdeleter,class = std::enable_if_t<std::is_convertible_v<_Up*,T*>>>
    Unique_ptr(Unique_ptr<_Up,_UDdeleter>&& other_ptr)noexcept:M_Ptr(other_ptr.M_Ptr){
        other_ptr.M_Ptr = nullptr;
    }
    //同类的移动构造函数
    Unique_ptr(Unique_ptr&& other_ptr)noexcept:M_Ptr(other_ptr.M_Ptr){
        other_ptr.M_Ptr = nullptr;
    }
    ~Unique_ptr(){
        M_deleter(M_Ptr);
    }

    //--------------一些工具和运算符重载------------------
    //移动赋值运算符
    Unique_ptr& operator=(Unique_ptr&& other_ptr)noexcept{
        if(this!=&other_ptr){
            if(M_Ptr) M_deleter(M_Ptr);
            M_Ptr = std::move(other_ptr.M_Ptr);
            other_ptr.M_Ptr= nullptr;
        }
        return *this;
    }

    void swap(Unique_ptr &__that) noexcept { // 交换函数
        std::swap(M_Ptr, __that.M_Ptr);
    }

    T* get() const noexcept{
        return M_Ptr;
    }

    //重载解->
    T* operator->()const noexcept{
        return M_Ptr;
    }

    //重载*运算符
    std::add_lvalue_reference_t<T> operator*() const{
        return *M_Ptr;
    }

    Deleter get_deleter() const noexcept{
        return M_deleter;
    }

    //将内部存储的指针给释放
    T* release() noexcept{
        T* temp = M_Ptr;
        M_Ptr = nullptr;
        return temp;
    }

    //重置智能指针
    void reset(T* ptr = nullptr) noexcept{
        T* old_ptr = M_Ptr;
        M_Ptr = ptr;
        if(old_ptr) M_deleter(old_ptr);
    }

    //重载bool运算符，判断智能指针是否持有资源
    explicit operator bool() const noexcept
    {
        return M_Ptr != nullptr;
    }

    bool operator==(Unique_ptr const &__that) const noexcept
    {
        return M_Ptr == __that.M_Ptr;
    }

    bool operator!=(Unique_ptr const &__that) const noexcept
    {
        return M_Ptr != __that.M_Ptr;
    }
private:
    T* M_Ptr;
    [[no_unique_address]]/*c++20*/ Deleter M_deleter;  //因为是个空类可以实现编译器优化让这个成员属性不占用内存
   
    //友元声明允许不同类型的UniquePtr之间访问彼此的私有成员，以实现跨类型的移动构造和赋值
    template <class _Up, class _UDeleter>
    friend struct Unique_ptr;
};


//偏特化数组类型的智能指针
template<typename T,typename Deleter>
struct Unique_ptr<T[],Deleter>:public Unique_ptr<T,Deleter> {
    using Unique_ptr<T,Deleter>::Unique_ptr; 

    std::add_lvalue_reference_t<T> operator[](std::size_t i) const{
        return this->get()[i];
    }
};

//智能指针还可以使用make_unique来创建对象,但是暂时先不实现