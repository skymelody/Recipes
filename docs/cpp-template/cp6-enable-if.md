1. 完美转发

   在cpp11后，对于函数的参数传递我们可以写四个版本。分别是值传递，引用左值引用传递，常量引用传递，右值引用传递

   ```cpp
   void foo(int x);
   void foo(const int& x);
   void foo(int& x);
   void foo(int&& x);
   ```

   对于右值引用版本的调用，只能传递临时量或者将亡量。

   ```cpp
   foo(1); // 调用foo(int&&)
   int x;
   foo(std::move(x)); //调用foo(int&&);
   
   // 如果只有foo(int&&)版本，下面的调用会报错
   int x;
   foo(x); // 编译出错！
   ```

   当写模板函数的时候，foo中调用了bar函数，并且bar的参数刚好是foo的实参的时候，如何保证参数的时候变量属性的不变形呢（常量任是常量，零时量任是临时量，引用任是引用）？即如下代码中，如何保证bar的参数性质的不变？

   ```cpp
   void bar(int x);
   void bar(int& x);
   void bar(int&& x);
   template <typename T>
   void foo(SOME_FORM_OF_T args){
       bar(args);
   }
   ```

   解决方案是万能引用结合完美转发

   ```cpp
   template <typename T>
   void f(T&& args){
       bar(std::forward<T>(args));
   }
   ```

   必须要结合完美转发，因为万能引用存在引用折叠的情况！

2. 万能引用和右值引用的区别

   万能引用能接受任何值，而右值引用只能接受将亡量和临时量

3. 当模板构造函数遇上拷贝构造函数

   ```cpp
   class Person {
   public:
   	template <typename STR>
       explicit Person(STR&& n) : name(std::forward<STR>(n)) {}
       Person(const Person& p) name(p.name) {}
   private:
       string name;
   };
   ```

   当我们有如下调用的时候，编译会出错！

   ```cpp
   string s = "name";
   Person p1(s);
   Person p2(p1); // 编译出错！
   ```

   以上的代码会编译出错。这是因为根据cpp的重载规则，非常量左值的模板构造函数的优先级要优于普通的拷贝构造函数！

   为了解决这个问题，我们应该考虑在某些情况下禁用模板构造函数。

4. enable_if

   enable_if的引入就是为了使我们能根据一些条件来禁用掉某些模板函数。

   ```cpp
   template <typename T>
   typename std::enable_if<(sizeof(T) > 4)>::type foo();
   ```

   当T的大小大于4的时候，foo的返回类型会被替换为enable_if的第二个模板参数。这里没有写，那就替换成默认的void。当小于4的时候，这是后模板实例化失败，按理说要编译报错，但标准说这不算是个错误SFINAE。

   在cpp14后上述代码可以写为

   ```cpp
   template <typename T>
   std::enable_if_t<(sizeof(T) > 4)> foo();
   ```

   除此之外，也可以把enable_if放入到模板参数中

   ```cpp
   template <typename T,
             typename RetType=std::enable_if_t<(sizeof(T)>4)>>
   RetType foo();
   ```

5. 使用enable_if改写Person

   ```cpp
   template <typename STR,
             typename = enable_if_t<is_convertible_v<STR, string>>>
   Person(STR&& n);
   ```

6. 如何禁用默认的拷贝构造而使用模板拷贝构造？

   当存在拷贝构造和模板拷贝构造时，重载规则会优先选择普通的拷贝构造。为了实现优先选择模板拷贝构造可考虑如下实现：

   ```cpp
   class C {
   public:
       C(const volatile& c) = delete;
       
       template <typename T>
       C(const T&);
   };
   ```

   这种情况下重载规则就会选择模板拷贝构造。