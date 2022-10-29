#include <iostream>

using namespace std;

 

class Father 
{
    public:
        void money(void) {
            cout << "I have Money" << endl;
        }
        int m_car;
        static int m_things;
    private:
        void wife(void) {
            cout << "I have wife" << endl;
        }
};

int Father::m_things = 100;

// 子类公有继承父类
class Son : public Father
{
    public:
        void car(void) {
            cout << "I have car" << endl;
        }
        int m_car;
        static int m_things;
};

int Son::m_things = 0;


int main(int argc, char **argv)
{
    Son s;
    s.money();

    s.m_car = 0;
    s.Father::m_car = 1;
    cout << "Son m_car is " << s.m_car << endl;
    cout << "Father m_car is " << s.Father::m_car << endl;

    s.m_things = 0;
    cout << "Son m_things is " << s.m_things << endl;
    s.Father::m_things = 1;
    cout << "Father m_things is " << s.Father::m_things << endl;
    Son::Father::m_things = 2; //静态同名成员，可以通过类名::作用域的方式访问
    cout << "Father m_things is " << Son::Father::m_things << endl;
}