#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>

// Adapted from: http://en.cppreference.com/w/cpp/thread/condition_variable

using namespace std;

mutex g_mutex;
condition_variable cv;
std::string g_data;
bool ready = false; // Shared variable
bool processed = false; // Shared variable

void Worker()
{
    unique_lock<mutex> lock(g_mutex);

    // Wait until main thread sends data.
    cv.wait(lock,[]{return ready;});
    // Equivalent to:
    // while (!ready) {
    //   cv.wait(lock);
    // }

    // After wait, we own the lock.
    cout<< "Worker thread is processing data..." << endl;
    // Sleep 1 second to simulate data processing.
    this_thread::sleep_for(chrono::seconds(1));
    g_data += " after processing";

    // Send data back to the main thread.
    processed = true;
    cout << "Worker thread signals data processing completed." << endl;

    // Manually unlock before notify to avoid waking up the waiting thread
    // only to block again (see notify_one for details).
    lock.unlock();

    cv.notify_one();
}

int main()
{
    thread worker(Worker);
    
    // Send data to the worker thread.
    {
        lock_guard<mutex> lock(g_mutex);
        cout << "Main thread is preparing data..." << endl;
        // Sleep 1 second to simulate data preparation.
        this_thread::sleep_for(chrono::seconds(1));
        g_data = "Example data";
        ready = true;
        std::cout << "Main thread signals data ready for processing." << std::endl;
    }
    cv.notify_one();

    // Wait for the worker thread to process data.
    {
        unique_lock<mutex> lock(g_mutex);
        cv.wait(lock,[]{return ready;});
    }
    cout << "Back in main thread, data = " << g_data << std::endl;

    worker.join();

    return 0;
}






