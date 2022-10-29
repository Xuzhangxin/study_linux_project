#include <iostream>
using namespace std;

class Person;

class Dog {
    public:
        int age;
        void visit_person();
        Dog();

    private:
        Person *m_person; //class Person是一个前向声明，是一个不完全类型，编译器不知道它包含某些成员，所以只能声明指针或引用，而不能声明对象
};

class Person {
    friend void func(); // 1)声明全局函数为类Person的友元，可以访问类的私有对象
    // friend class Dog;   // 2)声明类Dog是类Person的友元，可以访问类的私有对象
    friend void Dog::visit_person(); //3)声明成员函数为友元，可以访问类的私有对象
    public:
        int parlor;
        Person() {
            this->parlor = 1;
            this->room = 2;
        }
    private:
        int room;   //私有函数类外不允许访问
};

void Dog::visit_person()
{
    cout << this->m_person->room << endl;
}

Dog::Dog() 
{
    this->age = 18;
    m_person = new Person;
}

void func()
{
    Person p;
    cout << p.parlor << endl;
    cout << p.room << endl;

    Dog d;
    d.visit_person();

}
int main(int argc, char **argv)
{
    func();
}