#include <iostream>
using namespace std;

class Person {
    public:
        int m_a;
        static int m_b;
        static void func() { /// 静态成员函数只能访问静态成员变量
            // cout << " func, m_a:" << m_a << endl;
            cout << " func, m_b:" << m_b << endl;
        }
};

int Person::m_b = 10; // 静态成员变量类内定义类外初始化

int main(int argc, char **argv)
{
    Person p1;
    Person p2;

    cout << "p1 m_b:" << p1.m_b << endl; /// 所有对象共享静态成员变量
    cout << "p2 m_b:" << p2.m_b << endl;

    //cout << Person::m_a << endl;
    cout << Person::m_b << endl; /// 静态成员可通过对象访问，也可以通过类名访问
    Person::func();

    return 0;
}