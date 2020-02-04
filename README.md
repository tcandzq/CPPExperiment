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

### Mutex 4
为输出流使用单独的 mutex。
这么做是因为 IO 流并不是线程安全的！
如果不对 IO 进行同步，此例的输出很可能变成：

```cpp
count == count == 2count == 41
count == 3
```
因为在下面这条输出语句中：
```cpp
std::cout << "count == " << i << std::endl;
```
输出 "count == " 和 i 这两个动作不是原子性的（atomic），可能被其他线程打断。

```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex g_mutex;
std::mutex g_io_mutex;
int g_count = 0;

void Counter() {
  int i;
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    i = ++g_count;
  }

  {
    std::unique_lock<std::mutex> lock(g_io_mutex);
    std::cout << "count: " << i << std::endl;
  }
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
### 条件变量（Condition Variable）
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
工作线程：

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
主线程：
```cpp
int main() {
  std::thread worker(Worker);

  // 把数据发送给工作线程。
  {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "主线程正在准备数据..." << std::endl;
    // 睡眠一秒以模拟数据准备。
    std::this_thread::sleep_for(std::chrono::seconds(1));
    data = "样本数据";
    ready = true;
    std::cout << "主线程通知数据已经准备完毕。" << std::endl;
  }
  cv.notify_one();

  // 等待工作线程处理数据。
  {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [] { return processed; });
  }
  std::cout << "回到主线程，数据 = " << data << std::endl;

  worker.join();

  return 0;
}
```
下面是一些说明。

与条件变量搭配使用的「锁」，必须是 unique_lock，不能用 lock_guard。这个前面文章中已有说明。

等待前先加锁。等待时，如果条件不满足，wait 会原子性地解锁并把线程挂起。

条件变量被通知后，挂起的线程就被唤醒，但是唤醒也有可能是假唤醒，或者是因为超时等异常情况，所以被唤醒的线程仍要检查条件是否满足，所以 wait 是放在条件循环里面。cv.wait(lock, [] { return ready; }); 相当于：while (!ready) { cv.wait(lock); }。



## 基于 Asio 的线程池
由于要下载boost，暂时空着 参考：https://segmentfault.com/a/1190000006691692

## 生产者 - 消费者
生产者 - 消费者（Producer-Consumer），也叫有限缓冲（Bounded-Buffer），是多线程同步的经典问题之一。详见 [Wikipedia][1]。

代码改写自 Boost.Thread 自带的示例（`libs/thread/example/condition.cpp`），以「条件变量」实现同步。

### 头文件
```cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
```

### 有限缓冲类

```cpp
class BoundedBuffer {
public:
  BoundedBuffer(const BoundedBuffer& rhs) = delete;
  BoundedBuffer& operator=(const BoundedBuffer& rhs) = delete;

  BoundedBuffer(std::size_t size)
      : begin_(0), end_(0), buffered_(0), circular_buffer_(size) {
  }

  void Produce(int n) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      // 等待缓冲不为满。
      not_full_cv_.wait(lock, [=] { return buffered_ < circular_buffer_.size(); });

      // 插入新的元素，更新下标。
      circular_buffer_[end_] = n;
      end_ = (end_ + 1) % circular_buffer_.size();

      ++buffered_;
    }  // 通知前，自动解锁。

    // 通知消费者。
    not_empty_cv_.notify_one();
  }

  int Consume() {
    std::unique_lock<std::mutex> lock(mutex_);
    // 等待缓冲不为空。
    not_empty_cv_.wait(lock, [=] { return buffered_ > 0; });

    // 移除一个元素。
    int n = circular_buffer_[begin_];
    begin_ = (begin_ + 1) % circular_buffer_.size();

    --buffered_;

    // 通知前，手动解锁。
    lock.unlock();
    // 通知生产者。
    not_full_cv_.notify_one();
    return n;
  }

private:
  std::size_t begin_;
  std::size_t end_;
  std::size_t buffered_;
  std::vector<int> circular_buffer_;
  std::condition_variable not_full_cv_;
  std::condition_variable not_empty_cv_;
  std::mutex mutex_;
};
```

生产者与消费者线程共享的缓冲。`g_io_mutex` 是用来同步输出的。
```cpp
BoundedBuffer g_buffer(2);
boost::mutex g_io_mutex;
```

### 生产者
生产 100000 个元素，每 10000 个打印一次。
```cpp
void Producer() {
  int n = 0;
  while (n < 100000) {
    g_buffer.Produce(n);
    if ((n % 10000) == 0) {
      std::unique_lock<std::mutex> lock(g_io_mutex);
      std::cout << "Produce: " << n << std::endl;
    }
    ++n;
  }

  g_buffer.Produce(-1);
}
```

### 消费者
每消费到 10000 的倍数，打印一次。
```cpp
void Consumer() {
  std::thread::id thread_id = std::this_thread::get_id();

  int n = 0;
  do {
    n = g_buffer.Consume();
    if ((n % 10000) == 0) {
      std::unique_lock<std::mutex> lock(g_io_mutex);
      std::cout << "Consume: " << n << " (" << thread_id << ")" << std::endl;
    }
  } while (n != -1);  // -1 表示缓冲已达末尾。

  // 往缓冲里再放一个 -1，这样其他消费者才能结束。
  g_buffer.Produce(-1);
}
```

### 主程序

一个生产者线程，三个消费者线程。
```cpp
int main() {
  std::vector<std::thread> threads;

  threads.push_back(std::thread(&Producer));
  threads.push_back(std::thread(&Consumer));
  threads.push_back(std::thread(&Consumer));
  threads.push_back(std::thread(&Consumer));

  for (auto& t : threads) {
    t.join();
  }

  return 0;
}
```

输出（括号中为线程 ID）：
```
Produce: 0
Consume: 0 (13c0)
Produce: 10000
Consume: 10000 (15fc)
Produce: 20000
Consume: 20000 (2558)
Produce: 30000
Consume: 30000 (13c0)
Produce: 40000
Consume: 40000 (15fc)
Produce: 50000
Consume: 50000 (13c0)
Produce: 60000
Consume: 60000 (15fc)
Produce: 70000
Consume: 70000 (13c0)
Produce: 80000
Consume: 80000 (15fc)
Produce: 90000
Consume: 90000 (15fc)
```

### 分析

考虑一个生产者和一个消费者的情形，假定缓冲的大小为 2，来看看三个成员变量如何变化。
```
            buffered_    begin_      end_
 初始           0          0          0
 生产           1          0          1
 消费           0          1          1
 消费          等待 buffered_ > 0 ...
 生产           1          1          0
 ...
```

参考：
- [Wikipedia: Producer–consumer problem][2]
- [StackOverflow: Bounded Buffers (Producer Consumer)][3]
- [StackOverflow: Empty element in array-based bounded buffer][4]


  [1]: https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
  [2]: https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
  [3]: http://stackoverflow.com/questions/9578050/bounded-buffers-producer-consumer
  [4]: http://stackoverflow.com/questions/9517405/empty-element-in-array-based-bounded-buffer


## 信号量（Semaphore）

下面这段介绍，修改自 wxWidgets 官方文档（详见：[wxSemaphore Class Reference][1]）。

> **Semaphore** is a counter limiting the number of threads concurrently accessing a shared resource.

> This counter is always between 0 and the maximum value specified during the semaphore creation. When the counter is strictly greater than 0, a call to `Wait` returns immediately and decrements the counter. As soon as it reaches 0, any subsequent calls to Semaphore::Wait block and only return when the semaphore counter becomes strictly positive again as the result of calling `Signal` which increments the counter.

> In general, semaphores are useful to restrict access to a shared resource which can only be accessed by some fixed number of clients at the same time. For example, when modeling a hotel reservation system a semaphore with the counter equal to the total number of available rooms could be created. Each time a room is reserved, the semaphore should be acquired by calling `Wait` and each time a room is freed it should be released by calling `Signal`.

C++11 和 Boost.Thread 都没有提供信号量。对此 Boost 是这样解释的（[Why has class semaphore disappeared?][2]）：

> Semaphore was removed as too error prone. The same effect can be achieved with greater safety by the combination of a mutex and a condition variable. Dijkstra (the semaphore's inventor), Hoare, and Brinch Hansen all depreciated semaphores and advocated more structured alternatives. In a 1969 letter to Brinch Hansen, Wirth said "semaphores ... are not suitable for higher level languages." [Andrews-83] summarizes typical errors as "omitting a **P** or a **V**, or accidentally coding a **P** on one semaphore and a **V** on on another", forgetting to include all references to shared objects in critical sections, and confusion caused by using the same primitive for "both condition synchronization and mutual exclusion".

简单来说，就是信号量太容易出错了（too error prone），通过组合互斥锁（mutex）和条件变量（condition variable）可以达到相同的效果，且更加安全。实现如下：

```cpp
class Semaphore {
public:
  explicit Semaphore(int count = 0) : count_(count) {
  }

  void Signal() {
    std::unique_lock<std::mutex> lock(mutex_);
    ++count_;
    cv_.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [=] { return count_ > 0; });
    --count_;
  }

private:
  std::mutex mutex_;
  std::condition_variable cv_;
  int count_;
};
```

下面创建三个工作线程（Worker），来测试这个信号量。
```cpp
int main() {
  const std::size_t SIZE = 3;

  std::vector<std::thread> v;
  v.reserve(SIZE);

  for (std::size_t i = 0; i < SIZE; ++i) {
    v.emplace_back(&Worker);
  }

  for (std::thread& t : v) {
    t.join();
  }
  
  return 0;
}
```
每个工作线程先等待信号量，然后输出线程 ID 和当前时间，输出操作以互斥锁同步以防止错位，睡眠一秒是为了模拟线程处理数据的耗时。
```cpp
std::mutex g_io_mutex;

void Worker() {
  g_semaphore.Wait();

  std::thread::id thread_id = std::this_thread::get_id();

  std::string now = FormatTimeNow("%H:%M:%S");
  {
    std::lock_guard<std::mutex> lock(g_io_mutex);
    std::cout << "Thread " << thread_id << ": wait succeeded" << " (" << now << ")" << std::endl;
  }

  // Sleep 1 second to simulate data processing.
  std::this_thread::sleep_for(std::chrono::seconds(1));

  g_semaphore.Signal();
}
```

信号量本身是一个全局对象，`count` 为 `1`，一次只允许一个线程访问：
```cpp
Semaphore g_semaphore(1);
```
输出为：
```plain
Thread 1d38: wait succeeded (13:10:10)
Thread 20f4: wait succeeded (13:10:11)
Thread 2348: wait succeeded (13:10:12)
```
可见每个线程相隔一秒，即一次只允许一个线程访问。如果把 `count` 改为 `3`：
```cpp
Semaphore g_semaphore(3);
```
那么三个线程输出的时间应该一样：
```plain
Thread 19f8: wait succeeded (13:10:57)
Thread 2030: wait succeeded (13:10:57)
Thread 199c: wait succeeded (13:10:57)
```

最后附上 `FormatTimeNow` 函数的实现：
```cpp
std::string FormatTimeNow(const char* format) {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm* now_tm = std::localtime(&now_c);

  char buf[20];
  std::strftime(buf, sizeof(buf), format, now_tm);
  return std::string(buf);
}
```

参考：
- [C++0x has no semaphores? How to synchronize threads?][3]
- [Semaphore (programming)][4]


  [1]: http://docs.wxwidgets.org/trunk/classwx_semaphore.html
  [2]: http://www.boost.org/doc/libs/1_31_0/libs/thread/doc/faq.html
  [3]: http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
  [4]: https://en.wikipedia.org/wiki/Semaphore_%28programming%29#Semaphore_vs._mutex

## 读写锁（Read-Write Lock）
待更新参考：https://github.com/sprinfall/cpp-thread-study/blob/master/doc/CppConcurrency07.ReadWriteLock.md

## C++ 之bind使用
https://www.cnblogs.com/myd620/p/8706651.html