1. 值传递

   一般情况下建议把模板参数声明为值传递，除非

   * 无法拷贝
   * 参数将用作返回值
   * 模板转发时要保留参数原有的特性
   * 性能问题

   当我们使用值传递的时候，编译器会尽可能的帮我们优化，所以也不必太担心性能问题。比如cpp17要求传递纯右值的时候不允许发生拷贝。

2. 值传递将导致退化

   数组将退化成指针

3. 引用传递不会导致退化

   当传入一个数组或字符串的时候，模板参数类型会被推导为数组类型，而非指针类型。

   ```cpp
   template <typename T>
   void printR(const T& arg);
   printR("hi"); // => T推导为char[3]
   ```

4. 传递非常量引用

   一般当需要把返回值放在参数传出的时候用

   ```cpp
   template <typename T>
   void OutR(T& arg){
       arg = T();
   }
   ```

   注意，由于引用不会导致退化，所以如果这时候传进来的实参是常量的话，这个参数在函数中是read only的。

   ```cpp
   const int a = 1;
   OutR(a); // error!
   int b = 2;
   OutR(b); // ok;
   ```

   在写模板的时候，可能需要为模板加入一些限制来禁止传递常量引用。

   ```cpp
   template <typename T>
   void OutR(T& arg) {
       static_assert(!std::is_const<T>::value, "const arg is not allowed");
   }
   
   // alternative using enable_if
   template <typename T, typename = enable_if_t<!is_const<T>::value>>
   void OutR(T& arg)
   ```

5. 传递万能引用

   当我们需要完美转发参数的时候，把模板申明为万能引用。万能引用几乎是完美的，但也有一个小坑

   ```cpp
   template <typename T>
   void passR(T&& args){
       T x;
   }
   passR(1); //ok
   int i;
   passR(i); //err
   ```

   因为引用折叠，这里i变成了一个引用，所以无法在函数内初始化x;

6. std::cref和std::ref

   这两个函数让用户可以显式的将参数传递为引用和常量引用。当使用这两个函数的时候，编译器会帮我们生成一个reference_wrapper类作为参数传递给函数。在必要的时候（转发参数的时候）编译器又会帮我们转回原始类型。

   ```cpp
   void PrintStr(const string& str);
   template <typename T>
   void Print(T arg) {
       PrintStr(arg);
   }
   string s;
   Print(std::cref(s));
   ```

   注意！一般来说这两个函数只有在将对象通过模板传递给非模板函数时才能正常工作。直接在模板中使用这个包装过的对象可能会失败

   ```cpp
   template <typename T>
   void Print(T arg){
       cout << arg;
   }
   string s;
   Print(std::cref(s)); // error, since operator<< for wrapper_reference is not defined
   ```

7. 处理模板返回值

   一般来说我们必须确保函数返回的是一个值！因此当使用万能引用的时候要尤其注意。

   ```cpp
   template <typename T>
   T RetR(T&& p) {
       return T{};
   }
   ```

   这里T可能被折叠为引用类型，所以编译会报错。我们可以使用以下方法来解决这个问题

   ```cpp
   // 手动移除引用
   template <typename T>
   typename remove_reference<T>::type RetV(T p){
       return T{};
   }
   
   // 使用auto，让编译器来解决一切
   template <typename T>
   auto RetV(T p){
       return T{};
   }
   ```

   
