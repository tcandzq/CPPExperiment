#include<iostream>
#include<thread>

using namespace std;

// Create a thread by a function with no arguments.

void Hello()
{
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Hello,World" << endl;
}

int main()
{
    // Create a thread object, and Hello will run immediately.
    thread t(&Hello);

    t.join();

    return 0;
}
