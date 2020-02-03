#include<iostream>
#include<mutex>
#include<thread>
#include<vector>


// Use unique_lock to automatically lock and unlock a mutex.
// unique_lock provides more features than lock_guard.
// NOTE: mutex::scoped_lock is just a typedef of unique_lock<mutex>.

// unique_lock vs. lock_guard:
//   http://stackoverflow.com/questions/6731027/boostunique-lock-vs-boostlock-guard


using namespace std;

mutex g_mutex;
int g_count = 0;

void Counter()
{
    unique_lock<mutex> lock(g_mutex);

    int i = ++g_count;

    cout << "cout: " << i << endl;
}


int main()
{
    const size_t SIZE = 4;
    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        v.emplace_back(&Counter);
    }
    
    for(thread& t:v)
        t.join();

    return 0;
}