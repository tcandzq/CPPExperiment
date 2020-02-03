#include<iostream>
#include<thread>
#include <functional>
// Create a thread by a member function.

using namespace std;

class Hello
{
    public:
        Hello()
        {
            thread t(bind(&Hello::Entry,this,"World"));
            t.join();
        }
        

    private:
        void Entry(const char* what)
        {
            this_thread::sleep_for(chrono::seconds(1));
            
            cout << "Hello," << what << "!" << endl;
        }
};

int main()
{
    Hello hello;
}