#include<iostream>
#include<thread>

// Create a thread by a function object (functor).

using namespace std;

class Hello
{
    public:
        void operator()(const char* what)
        {
            this_thread::sleep_for(chrono::seconds(1));
            cout << "Hello" << what << "!" << endl;
        }
};

int main()
{
    Hello hello;

    thread t1(hello,"World1");
    t1.join();

    // Don't copy the thread object, use std::ref to pass in a reference.
    // But the user must ensure that the referred-to object outlives the
    // newly-created thread of execution.

    thread t2(ref(hello),"World2");
    t2.join();

    return 0;

}