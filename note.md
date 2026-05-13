# stl的六大组成
1. allocator  -> 负责内存管理
2. iterator
3. container
4. functor
5. algorithm
6. adapter

# allocator
SGI的allocator类定义在头文件<memory>,而<memory>头文件又依赖于<alloc.h>,<construct.h>,<uninitialized.h>三个头文件SS
## <construct.h>:
主要负责对象的构造和析构。即construct()和destroy().
在析构之前会先查看对象的类型是否有 **trivial destructor**S
> trivial desturctor: 是否有自己定义而不是编译器生成的析构函数

## <alloc.h>
真正负责分配内存的模块
内置两级
# iterator
## 前闭后开的区间表示法
如果使用一对迭代器来表示区间 [first,last)
当first == last 的时候刚好表示的是空区间