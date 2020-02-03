#include<iostream>
#include<thread>

using namespace std;

class Counter
{
    public:
        Counter(int value):value_(value){
            
        }
        void operator()()
        {
            while (value_ > 0)
            {
                cout << value_ << " ";
                --value_;
                this_thread::sleep_for(chrono::seconds(1));
            }
            cout << endl;
        }
    private:
        int value_;
};

int main()
{
    thread t1(Counter(3));
    t1.join();

    thread t2(Counter(3));
    t2.detach();

    // Wait for the detached t2 to finish.

    this_thread::sleep_for(chrono::seconds(4));

    return 0;

}