#include <iostream>
using namespace std;

class Person {
    public:
        int m_a;
        int m_b;
        int m_c;
    // 默认有参构造函数
    Person(int a, int b , int c) :m_a(a),m_b(b),m_c(c) {

    }
};


int main(int argc, char **argv)
{
    Person p1(30,20,10);
    cout << p1.m_a << p1.m_b << p1.m_c << endl;

    return 0;
}