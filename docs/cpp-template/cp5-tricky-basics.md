1. 当使用模板参数中的类型时需要加上typename

   ```cpp
   template <typename T>
   class Test {
   public:
       void foo(){
           typename T::SubType* val;
       }
   }
   ```

   如果不加上typename关键字的话，编译器无法知道SubType是一个静态变量还是类型。如果不加的话，某些情况下也是可也编译通过的，这时候表示两个值的乘积。

2. 模板变量初始化

   对于基础类型，模板变量值的初始化也是随机的。因此可用()或者{}来进行默认初始化。

3. 使用this->来访问模板类的成员

   当某个模板类继承自某个模板基类的时候，需要用this->来访问模板基类中的成员

   ```cpp
   template <typename T>
   class Base {
       public:
       void bar();
   };
   template <typename T>
   class Derived : public Base<T> {
       public:
       void foo() {
           this->bar(); // use this here
       }
   };
   ```

   如果没有使用this->的话，那么将会调用全局的bar或者其他的bar函数，总之不会调到Base中的

4. 数组和字符串模板

   当模板参数是数组的引用时，传入的数组实参不会退化成指针，因此这种情况下就要处理好模板函数参数的申明，因为不同大小的数组是不同的类型！

   ```cpp
   template <typename T, int N, int M>
   bool less(T(&a)[N], T(&b)[M]) {
       for (int i = 0; i<N&&i<M; ++i) {
           if (a[i] < b[i]) return true;
           if (b[i] < a[i]) return false;
       }
       return N < M;
   }
   ```

5. 模板构造函数或模板赋值函数不会影响到普通的构造函数和赋值函数

   ```cpp
   class Foo {
       public:
       Foo(int a);
       template <typename T>
       Foo(T t);
   };
   ```

   这种情况下Foo(1)任然调的是普通的构造函数

6. dot template 构造

   ```cpp
   template <unsigned long N>
   void print_bitset(const bitset<N>& bs){
       cout << bs.template to_string<char, char_traits<char, allocator<char>>();
   }
   ```

   如果没有.template的话，编译器会把to_string后的<当做是小于号处理，而不是模板开始的token。这个问题只存在于构造依赖于模板参数的情况，在这里bs依赖于N。

7. 泛型lambda

   c++14推出，是类模板成员的缩写

   ```cpp
   [](auto x, auto y){
       return x + y;
   }
   // equals to
   class Tmp {
       public:	
       template <typename T1, typename T2>
       auto operator()(T1 x, T2 y) {
           return x + y;
       }
   };
   ```

8. 模板变量

   ```cpp
   template <typename T>
   T val{3.1};
   template <typename T>
   T val1{};
   
   int main() { 
   	cout << val<double> << endl;
       val<string> = "abc";
       cout << val<string> << endl;
   }
   ```

   模板变量不能出现在函数体或者块作用域中。模板变量的优点是可以简化一些代码的写法。

   ```cpp
   template <typename T>
   class Foo {
     	public:
       static constexpr int max = 1000;
   };
   template <typename T>
   int MAX = Foo<T>::max;
   // Foo<T>::max ==> MAX<T>;
   
   // stdlib
   // std::is_const<T>::value ==> std::is_const_v<T>
   ```

9. 模板模板参数

   对于Stack\<int, std::vector\<int\>\>，我们想省略容器vector中的int参数该怎么办？因为已经明确了Stack中存的类型是int，所以就没必要在vector中再写这个参数了。改怎么办？考虑如下写法

   ```cpp
   template <typename T, 
             template <typename Elem, 
                       typename Alloc = std::allocator<Elem>>
              class Cont=std::deque>
   class Stack {};
   ```

   在c++17之前，deque中的默认模板参数Alloc是必须申明的，在cpp17之后Alloc可去掉。那么我们在调用的时候

   ```cpp
   Stack<int, vector> s;
   ```

   即可。注意这个时候如果写成Stack\<int, vector\<\<int\>\> 是错误的！

   

