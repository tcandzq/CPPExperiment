# CPPExperiment
C++的一些实验项目(主要在Windows环境中)

## CMakeLists学习
C++ CMakeLists的Hello World版本，[参考代码](learn-cmake/HelloWorld/CMakeLists.txt)

需要安装以下软件：
- mingw-w64
- CMake

注意事项：
> 安装了mingw-w64，把bin下的mingw32-make.exe复制一份并改名为make.exe。



Ctrl+Shift+P 输入下面的命令为CMake指定编译工具链
> CMake:Select a Kit

运行的命令如下：
- cd build
- make
- 直接输入**.exe文件即可

[参考1](https://blog.csdn.net/qq_39942341/article/details/98964620)

[参考2](https://blog.csdn.net/MakerCloud/article/details/93545513)

## Windows使用C++调用Pytorch1.0模型

[参考代码](example-app/CMakeLists.txt)

注意：
Libtorch要下载稳定版，不要下载Nightly版本的。否则会发生"找不到Torch所在路径"的错误。


参考：
[pyTorch官网的Minimal Example](https://pytorch.org/cppdocs/installing.html)
[Windows使用C++调用Pytorch1.0模型](https://zhuanlan.zhihu.com/p/52806730)

## C++ 实现的一些小工具


## C++ 多线程编程
## thread
### thread1
通过一个不带参数的函数创建线程。
```cpp
#include <iostream>
#include <thread>

void Hello() {
  // 睡眠一秒以模拟数据处理。
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Hello, World!" << std::endl;
}

int main() {
  // 创建一个线程对象，注意函数 Hello 将立即运行。
  std::thread t(&Hello);

  // 等待线程结束。
  // 否则线程还没执行（完），主程序就已经结束了。
  t.join();

  return 0;
}

```
### thread2
通过一个带参数的函数创建线程。
```cpp
#include <iostream>
#include <thread>

void Hello(const char* what) {
  // 睡眠一秒以模拟数据处理。
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Hello, " << what << "!" << std::endl;
}

int main() {
  std::thread t(&Hello, "World");
  
  // 等价于使用 bind：
  //   std::thread t(std::bind(&Hello, "World"));

  t.join();

  return 0;
}
```
### thread3
通过一个函数对象——即仿函数（functor）——创建线程。
```cpp
#include <iostream>
#include <thread>

class Hello {
public:
  void operator()(const char* what) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Hello, " << what << "!" << std::endl;
  }
};

int main() {
  Hello hello;

  // 方式一：拷贝函数对象。
  std::thread t1(hello, "World");
  t1.join();

  // 方式二：不拷贝函数对象，通过 boost::ref 传入引用。
  // 用户必须保证被线程引用的函数对象，拥有超出线程的生命期。
  // 比如这里通过 join 线程保证了这一点。 
  std::thread t2(std::ref(hello), "World");
  t2.

  return 0;
}
```
### thread4
通过一个成员函数创建线程。
与前例不同之处在于，需要以 bind 绑定 this 指针作为第一个参数。
```cpp
#include <iostream>
#include <thread>

class Hello {
public:
  Hello() {
    std::thread t(std::bind(&Hello::Entry, this, "World"));
    t.join();
  }

private:
  // 线程函数
  void Entry(const char* what) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Hello, " << what << "!" << std::endl;
  }
};

int main() {
  Hello hello;
  return 0;
}

```

### Counter
创建两个线程，各自倒着计数。
此例顺带演示了 detached 线程，被 detached 的线程，自生自灭，不受控制，无法再 join。
```cpp
include <iostream>
#include <thread>

class Counter {
public:
  Counter(int value) : value_(value) {
  }

  void operator()() {
    while (value_ > 0) {
      std::cout << value_ << " ";
      --value_;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << std::endl;
  }

private:
  int value_;
};

int main() {
  std::thread t1(Counter(3));
  t1.join();

  std::thread t2(Counter(3));
  t2.detach();

  // 等待几秒，不然 t2 根本没机会执行。
  std::this_thread::sleep_for(std::chrono::seconds(4));
  
  return 0;
}
```

## Mutex（互斥锁）
多个线程访问同一资源时，为了保证数据的一致性，最简单的方式就是使用 mutex（互斥锁）。

引用 cppreference 的介绍：
>The mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
### Mutex 1
直接操作 mutex，即直接调用 mutex 的 lock / unlock 函数。
```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


std::mutex g_mutex;
int g_count = 0;

void Counter() {
  g_mutex.lock();

  int i = ++g_count;
  std::cout << "count: " << i << std::endl;

  // 前面代码如有异常，unlock 就调不到了。
  g_mutex.unlock();
}

int main() {
  const std::size_t SIZE = 4;

  // 创建一组线程。
  std::vector<std::thread> v;
  v.reserve(SIZE);

  for (std::size_t i = 0; i < SIZE; ++i) {
    v.emplace_back(&Counter);
  }

  // 等待所有线程结束。
  for (std::thread& t : v) {
    t.join();
  }

  return 0;
}

```
可惜的是，STL 没有提供 boost::thread_group 这样代表一组线程的工具，通过 std::vector 固然也能达到目的，但是代码不够简洁。

###  Mutex 2
使用 `lock_guard` 自动加锁、解锁。原理是 RAII，和智能指针类似。
```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex g_mutex;
int g_count = 0;

void Counter() {
  // lock_guard 在构造函数里加锁，在析构函数里解锁。
  std::lock_guard<std::mutex> lock(g_mutex);

  int i = ++g_count;
  std::cout << "count: " << i << std::endl;
}

int main() {
  const std::size_t SIZE = 4;

  std::vector<std::thread> v;
  v.reserve(SIZE);

  for (std::size_t i = 0; i < SIZE; ++i) {
    v.emplace_back(&Counter);
  }

  for (std::thread& t : v) {
    t.join();
  }

  return 0;
}

```
###  Mutex 3
使用 unique_lock 自动加锁、解锁。 unique_lock 与 lock_guard 原理相同，但是提供了更多功能（比如可以结合条件变量使用）。 注意：mutex::scoped_lock 其实就是 unique_lock<mutex> 的 typedef。

至于 unique_lock 和 lock_guard 详细比较，可移步 [StackOverflow](http://stackoverflow.com/questions/6731027/boostunique-lock-vs-boostlock-guard)。
```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex g_mutex;
int g_count = 0;

void Counter() {
  std::unique_lock<std::mutex> lock(g_mutex);

  int i = ++g_count;
  std::cout << "count: " << i << std::endl;
}

int main() {
  const std::size_t SIZE = 4;

  std::vector<std::thread> v;
  v.reserve(SIZE);

  for (std::size_t i = 0; i < SIZE; ++i) {
    v.emplace_back(&Counter);
  }

  for (std::thread& t : v) {
    t.join();
  }

  return 0;
}

```

## 条件变量（Condition Variable）
条件变量（Condition Variable）的一般用法是：线程 A 等待某个条件并挂起，直到线程 B 设置了这个条件，并通知条件变量，然后线程 A 被唤醒。经典的「生产者-消费者」问题就可以用条件变量来解决。

这里等待的线程可以是多个，通知线程可以选择一次通知一个（notify_one）或一次通知所有（notify_all）。

示例修改自：http://en.cppreference.com/w/cpp/thread/condition_variable

首先是头文件：
```cpp
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
```
然后是两个线程共享的全局变量：
```cpp
std::mutex mutex;
std::condition_variable cv;
std::string data;
bool ready = false;  // 条件
bool processed = false;  // 条件
```
工作流程：

```cpp
void Worker() {
  std::unique_lock<std::mutex> lock(mutex);

  // 等待主线程发送数据。
  cv.wait(lock, [] { return ready; });

  // 等待后，继续拥有锁。
  std::cout << "工作线程正在处理数据..." << std::endl;
  // 睡眠一秒以模拟数据处理。
  std::this_thread::sleep_for(std::chrono::seconds(1));
  data += " 已处理";

  // 把数据发回主线程。
  processed = true;
  std::cout << "工作线程通知数据已经处理完毕。" << std::endl;

  // 通知前，手动解锁以防正在等待的线程被唤醒后又立即被阻塞。
  lock.unlock();

  cv.notify_one();
}
```


## C++ 之bind使用
https://www.cnblogs.com/myd620/p/8706651.html