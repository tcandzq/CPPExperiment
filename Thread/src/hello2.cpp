#include<iostream>
#include<thread>


using namespace std;

// Create a thread by a function with argument(s).

void Hello(const char* what)
{
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Hello," << what << "!" << endl;
}

int main()
{
    thread t(&Hello,"World");

    // Equivalent to:
    //   std::thread t(std::bind(&Hello, "World"));

    t.join();
    return 0;   
}