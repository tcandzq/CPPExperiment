#include<vector>
#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<ctime>
#include<string>


// Implement semaphore based on mutex and condition variable.

// Adapted from:
// http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads

// Boost: Why has class semaphore disappeared?
// http://www.boost.org/doc/libs/1_31_0/libs/thread/doc/faq.html

// https://en.wikipedia.org/wiki/Semaphore_%28programming%29#Semaphore_vs._mutex


using namespace std;

class Semaphore
{
    public:
        explicit Semaphore(int count =0):count_(count){

        };

    void Signal()
    {
        unique_lock<mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void Wait()
    {
        unique_lock<mutex> lock(mutex_);
        cv_.wait(lock,[=]{return count_ > 0;});
        --count_;
    }


    private:
        mutex mutex_;
        condition_variable cv_;
        int count_;
};

Semaphore g_semaphore(3);
mutex g_io_mutex;

string ForwatTinmeNow(const char* format)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* now_tm = localtime(&now_c);

    char buf[20];
    strftime(buf,sizeof(buf),format,now_tm);
    return string(buf);
}

void Worker()
{
    g_semaphore.Wait();
    
    thread::id thread_id = this_thread::get_id();

    string now = ForwatTinmeNow("%H:%M:%S");
    {
        lock_guard<mutex> lock(g_io_mutex);
        cout << "Thread " << thread_id << ": wait succeed" << "(" << now << ")" << endl;
        
    }
    // Sleep 1 second to simulate data processing.
    this_thread::sleep_for(chrono::seconds(1));

    g_semaphore.Signal();
}

int main()
{
    const size_t SIZE = 3;

    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        v.emplace_back(&Worker);
    }
    
    for(thread& t:v)
        t.join();
    return 0;
}
