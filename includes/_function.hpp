#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <typeinfo>
#include <functional>

namespace MySTL
{
    // 当函数标签输入的不对时匹配
    //  特例化没有匹配击中主模板直接报错
    template <typename _FncSig>
    struct Function
    {
        // 始终报错。但是为了触发惰性编译只有让条件依赖于模板参数才行
        static_assert(std::is_same_v<_FncSig, _FncSig>, "not a valid function signature");
    };

    template <class Ret, class... Args>
    class Function<Ret(Args...)>
    {
        //
        struct _FuncBase
        {
            virtual Ret _M_Call(Args... agrs) = 0;                   // 执行函数的逻辑部分
            virtual std::unique_ptr<_FuncBase> _M_clone() const = 0; // 克隆当前的函数对象
            virtual std::type_info const &_M_type() const = 0;
            virtual ~_FuncBase() = default;
        };

        template <class _Fn> // 这个类型参数就是三种函数对象
        struct _FuncImpl : _FuncBase
        {
            _Fn _M_f; // 函数对象

            template <class... CArgs>
            explicit _FuncImpl(std::in_place_t, CArgs &&...args) : _M_f(std::forward<CArgs>(args)...) {}

            Ret _M_Call(Args... args) override
            {
                // 标准库给的统一调用函数接口 C++17万能调用器
                return std::invoke(_M_f, std::forward<Args>(args)...);
            }

            std::unique_ptr<_FuncBase> _M_clone() const override
            {
                // 传入参数in_place，编译器会自动寻找利用了in_place_t的构造函数
                return std::make_unique<_FuncImpl>(std::in_place, _M_f);
            }

            // 用于在多态中确定具体的类型
            std::type_info const &_M_type() const override
            {
                return typeid(_Fn);
            }
        };

        // 存基类的指针根据里氏替换原则就可以存储子类啦
        std::unique_ptr<_FuncBase> _M_base;

    public:
        Function() = default;
        Function(std::nullptr_t) noexcept : Function() {} // 使用了委托构造函数

        template <class _Fn, class = std::enable_if_t<std::is_invocable_r_v<Ret, std::decay_t<_Fn>, Args...> && std::is_copy_constructible_v<_Fn> && !std::is_same_v<std::decay_t<_Fn>, Function<Ret(Args...)>>>>
        Function(_Fn &&_f) : _M_base(std::make_unique<_FuncImpl<std::decay_t<_Fn>>>(std::in_place, std::forward<_Fn>(_f)))
        {
        }

        Function(Function &&) = default;
        Function &operator=(Function &&) = default;

        Function(Function const &__that) : _M_base(__that._M_base ? __that._M_base->_M_clone() : nullptr) {}
        Function &operator=(Function const &__that)
        {
            if (__that._M_base)
                _M_base = __that._M_base->_M_clone();
            else
                _M_base = nullptr;
        }

        explicit operator bool() const noexcept
        {
            return _M_base != nullptr;
        }

        bool operator==(std::nullptr_t) const noexcept
        {
            return _M_base == nullptr;
        }

        bool operator!=(std::nullptr_t) const noexcept
        {
            return _M_base != nullptr;
        }

        Ret operator()(Args... __args) const
        {
            if (!_M_base) [[unlikely]] // 告诉编译器这个条件分支很少执行
                throw std::bad_function_call();
            // 完美转发所有参数，这样即使 Args 中具有引用，也能不产生额外的拷贝开销
            return _M_base->_M_Call(std::forward<Args>(__args)...);
        }

        std::type_info const &target_type() const noexcept
        {
            return _M_base ? _M_base->_M_type() : typeid(void);
        }

        // 获取类型的目标指针
        template <class _Fn>
        _Fn *target() const noexcept
        {
            return _M_base && typeid(_Fn) == _M_base->_M_type() ? std::addressof(static_cast<_FuncImpl<_Fn> *>(_M_base.get())->_M_f) : nullptr;
        }

        void swap(Function &__that) const noexcept
        {
            _M_base.swap(__that._M_base);
        }
    };
};