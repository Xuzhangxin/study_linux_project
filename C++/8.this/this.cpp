#include <iostream>
using namespace std;

class Person { 
    public: 
        Person() { 
            mA = 0; 
    }
    //非静态成员变量占对象空间 
    int mA;
    //静态成员变量不占对象空间 
    static int mB; 
    //函数也不占对象空间，所有函数共享一个函数实例 
    void func() { }
    //静态成员函数也不占对象空间 
    static void sfunc() { } 
};


int main(int argc, char **argv)
{
    cout << sizeof(Person) << endl;

    return 0;
}