# 类型擦除与 Function 实现

## 设计目标

用一个统一的接口来操作不同类型的对象，就像 `std::function` 一样，可以将函数指针、lambda 表达式、仿函数作为参数而达到相同的效果。

## 设计思路

### 核心架构

采用 **三层架构** 实现类型擦除：

```
┌─────────────────────────────────────────────────────────┐
│  Function<Ret(Args...)>  ── 对外统一接口                 │
│       ↓                                                 │
│  ┌─────────────────────────────────────────────────┐    │
│  │  _FuncBase  ── 抽象基类（统一接口）              │   │
│  │  ├─ virtual _M_call(Args...) = 0              │   │
│  │  ├─ virtual _M_clone() = 0                   │   │
│  │  └─ virtual _M_type() = 0                    │   │
│  └─────────────────────────────────────────────────┘   │
│       ↓                                                │
│  ┌─────────────────────────────────────────────────┐   │
│  │  _FuncImpl<_Fn>  ── 模板派生类（具体实现）       │   │
│  │  ├─ _Fn _M_f;  // 存储具体函数对象             │   │
│  │  ├─ _M_call()  // 调用 _M_f                   │   │
│  │  ├─ _M_clone() // 克隆自身                     │   │
│  │  └─ _M_type()  // 返回 typeid(_Fn)            │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 关键组件

#### 1. 抽象基类 `_FuncBase`

定义统一的虚函数接口，实现类型擦除：

```cpp
struct _FuncBase {
    virtual _Ret _M_call(_Args... __args) = 0;               // 执行调用
    virtual std::unique_ptr<_FuncBase> _M_clone() const = 0; // 克隆对象
    virtual std::type_info const &_M_type() const = 0;       // 获取类型信息
    virtual ~_FuncBase() = default;                          // 虚析构函数
};
```

**设计要点**：
- 通过纯虚函数实现多态
- 虚析构函数确保正确析构派生类对象

#### 2. 模板派生类 `_FuncImpl`

存储具体函数对象，实现类型保留：

```cpp
template <class _Fn>
struct _FuncImpl : _FuncBase {
    _Fn _M_f;  // 存储具体类型的函数对象
    
    _Ret _M_call(_Args... __args) override {
        return std::invoke(_M_f, std::forward<_Args>(__args)...);
    }
    
    std::unique_ptr<_FuncBase> _M_clone() const override {
        return std::make_unique<_FuncImpl>(std::in_place, _M_f);
    }
    
    std::type_info const &_M_type() const override {
        return typeid(_Fn);
    }
};
```

**设计要点**：
- `_Fn` 可以是任意可调用类型（函数指针、lambda、仿函数）
- `std::invoke` 统一调用各种可调用对象
- `std::in_place_t` 标签避免额外拷贝

#### 3. 对外接口 `Function`

提供统一的用户接口：

```cpp
template <class _Ret, class... _Args>
struct Function<_Ret(_Args...)> {
    std::unique_ptr<_FuncBase> _M_base;  // 基类指针实现多态
    // ...
};
```

### 类型擦除核心

**构造函数**（第64-68行）是实现类型擦除的关键：

```cpp
template <class _Fn, class = std::enable_if_t<
    std::is_invocable_r_v<_Ret, std::decay_t<_Fn>, _Args...>
    && std::is_copy_constructible_v<_Fn>
    && !std::is_same_v<std::decay_t<_Fn>, Function<_Ret(_Args...)>>
>>
Function(_Fn &&__f)
    : _M_base(std::make_unique<_FuncImpl<std::decay_t<_Fn>>>(std::in_place, std::forward<_Fn>(__f)))
```

**三个条件**：
1. `is_invocable_r_v` - 检查是否可调用并返回正确类型
2. `is_copy_constructible_v` - 检查是否可拷贝（用于 clone）
3. `!is_same_v` - 防止自我赋值

## 工作流程

```
用户代码:
Function<int(int)> f = [](int x) { return x * 2; };

内部流程:
1. 构造函数推导出 _Fn = lambda类型
2. SFINAE 检查：is_invocable_r_v<int, lambda_type, int> → true
3. 创建 _FuncImpl<lambda_type> 对象
4. _M_base 指向该对象（基类指针 → 派生类对象）
5. 调用 f(5): _M_base->_M_call(5) → std::invoke(lambda, 5)
```

## 关键技术点

| 技术 | 作用 | 位置 |
|------|------|------|
| **模板特化** | 处理函数签名模式 `Ret(Args...)` | 第17-18行 |
| **SFINAE** | 通过 enable_if 约束构造函数 | 第64行 |
| **完美转发** | 保持参数的左值/右值特性 | 第66行 |
| **类型擦除** | 通过虚函数和基类指针实现 | 第56行 |
| **原型模式** | 通过 clone 实现深拷贝 | 第45-48行 |
| **std::invoke** | 统一调用各种可调用对象 | 第42行 |

## 设计优势

1. **类型安全**：编译期检查可调用性
2. **零拷贝**：使用 `std::in_place_t` 就地构造
3. **多态支持**：通过虚函数实现运行时多态
4. **可扩展性**：轻松支持新的可调用类型

## 总结

该实现通过 **"抽象基类 + 模板派生类 + 基类指针"** 的经典模式实现了类型擦除，核心在于：
- 用 `_FuncBase` 定义统一接口
- 用 `_FuncImpl<_Fn>` 存储具体类型
- 用 `std::enable_if` + `is_invocable_r` 约束可调用类型
- 用 `std::invoke` 统一调用各种函数对象
