#include<iostream>
#include<mutex>
#include<thread>
#include<vector>

// Use a individual mutex for output stream.

using namespace std;


mutex g_mutex;
mutex g_io_mutex;
int g_count = 0;

void Counter()
{
    int i;
    {
        unique_lock<mutex> lock(g_mutex);
        i = ++g_count;
    }
    
    {
        // The IO streams are not guaranteed to be thread-safe!
        // If you don't synchronize the IO, the output might look like:
        // count == count == 2count == 41
        // count == 3
        // Which means the output of "count == " and i is not atomic.
        unique_lock<mutex> lock(g_io_mutex);
        cout << "count: " << i << endl; 
    }
    
}

int main()
{
    size_t SIZE = 4;

    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        v.emplace_back(&Counter);  
    }
    for(thread& t:v)
    {
        t.join();
    }
    return 0;
}
