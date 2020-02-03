#include<iostream>
#include<mutex>
#include<thread>
#include<vector>

using namespace std;

mutex g_mutex;
int g_count = 0;


void Counter()
{
    g_mutex.lock();

    int i = ++g_count;
    cout << "cout: " << i << endl;

    g_mutex.unlock();
}

int main()
{
    const size_t SIZE = 4;
    // Create a group of counter threads.
    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        v.emplace_back(&Counter);
    }

    //Wait for all the threads to finish.
    for(thread& t:v)
        t.join();
    
    return 0;
}