#include<condition_variable>
#include<thread>
#include<vector>
#include<mutex>
#include<iostream>



using namespace std;

// The bounded-buffer problem, also known as producer¨Cconsumer.

// See:
// https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
// http://stackoverflow.com/questions/9578050/bounded-buffers-producer-consumer
// http://stackoverflow.com/questions/9517405/empty-element-in-array-based-bounded-buffer

// Consider one producer and one consumer, the buffer size is 2.
//            buffered_    begin_      end_
// Init           0          0          0
// Produce        1          0          1
// Consume        0          1          1
// Consume       Wait for buffered_ > 0 ...
// Produce        1          1          0
// ...

class BoundedBuffer
{
    public:
        BoundedBuffer(const BoundedBuffer& rhs) = delete;
        BoundedBuffer& operator=(const BoundedBuffer& rhs) = delete;

        BoundedBuffer(std::size_t size)
            :begin_(0),end_(0),buffered_(0),circular_buffer_(size){
        }
    
    void Produce(int n)
    {
        {
            unique_lock<mutex> lock(mutex_);
            not_full_cv_.wait(lock,[=]{return buffered_ < circular_buffer_.size();});

            circular_buffer_[end_] = n;

            end_ = (end_+1) % circular_buffer_.size();

            ++buffered_;
        }
        not_empty_cv_.notify_one();
    }

    int Consume()
    {
        unique_lock<mutex> lock(mutex_);
        not_empty_cv_.wait(lock,[=]{return buffered_ >0;});

        int n = circular_buffer_[begin_];
        begin_ = (begin_ + 1) % circular_buffer_.size();

        --buffered_;

        lock.unlock();
        not_full_cv_.notify_one();
        return n;
    }
    private:
        size_t begin_;
        size_t end_;
        size_t buffered_;
        vector<int> circular_buffer_;
        condition_variable not_full_cv_;
        condition_variable not_empty_cv_;
        mutex mutex_;
};

BoundedBuffer g_buffer(2);
mutex g_io_mutex;

void Producer()
{
    int n = 0;
    while (n < 100000)
    {
        g_buffer.Produce(n);
        if ((n % 10000) == 0)
        {
            unique_lock<mutex> lock(g_io_mutex);
            cout << "Produce: " << n << endl;
        }
        ++n;
    }
    g_buffer.Produce(-1);
}

void Consumner()
{
    thread::id thread_id = this_thread::get_id();

    int n = 0;
    do
    {
        n = g_buffer.Consume();
        if ((n % 10000) == 0)
        {
            unique_lock<mutex> lock(g_io_mutex);
            cout << "Consume: " << n << " (" << thread_id << ")" << endl;
        }
        
    } while (n != -1); // -1 indicates end of buffer.
    
    g_buffer.Produce(-1); // For stopping next consumer.
}

int main()
{
    vector<thread> threads;

    threads.push_back(thread(&Producer));
    threads.push_back(thread(&Consumner));
    threads.push_back(thread(&Consumner));
    threads.push_back(thread(&Consumner));
    
    for(auto& t: threads)
        t.join();

    return 0;
}