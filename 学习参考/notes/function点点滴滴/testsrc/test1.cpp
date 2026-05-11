int main()
{
  int a=10;
  int b=20;
  auto add = [a,b]()->int{
    return a+b;};
}

//本质版本
int main()
{
  int a = 10;
  int b = 20;
    
  class __lambda_7_14
  {
    public: 
    inline /*constexpr */ int operator()() const
    {
      return a + b;
    }
    
    private: 
    int a;
    int b;
    
    public:
    __lambda_7_14(int & _a, int & _b)
    : a{_a}
    , b{_b}
    {}
    
  };
  
  __lambda_7_14 add = __lambda_7_14{a, b};
  return 0;
}
