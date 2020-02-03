#include<iostream>
#include<mutex>
#include<thread>
#include<vector>


// Use lock_guard to automatically lock and unlock a mutex.

using namespace std;

mutex g_mutex;

int g_count = 0;

void Counter()
{
    lock_guard<mutex> lock(g_mutex);

    int i = ++g_count;
    cout << "cout: " << i << endl;
}

int main()
{
    size_t SIZE = 4;

    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; i++)
    {
        v.emplace_back(&Counter);
    }
    
    for(thread& t:v)
    {
        t.join();
    }
    
    return 0;
}