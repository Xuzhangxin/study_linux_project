#include <iostream>
#include <memory>
using namespace std;



int main(int argc, char **argv)
{
    unique_ptr<int> uni1(new int(1));

    unique_ptr<int> uni2(uni1.release());   /// uni1放弃对指针的控制权，返回指针，并将uni1置为空

    unique_ptr<int> uni3(new int(3));

    uni1.reset(new int(4));                 /// 释放uni1指向的对象，并让uni1指向新的对象
    uni1.reset(nullptr);                    /// 释放uni1指向的对象，并让uni1指向null         

    cout << uni1.get() <<endl;
    if (uni1.get() != NULL)
    {
        cout << *uni1 <<endl;
    }

    cout << uni2.get() <<endl;
    if (uni2.get() != NULL)
    {
        cout << *uni2 <<endl;
    }
}