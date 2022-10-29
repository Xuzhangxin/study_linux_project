#include <iostream>
using namespace std;

class Animal {
    public:
        Animal() {
            cout << "Animal 构造函数调用" << endl;
        }

        virtual ~Animal() = 0;

};

Animal::~Animal()
{
    cout << "Animal 析构函数调用" << endl;
}

class Dog : public Animal {
    public:
        Dog(string name) {
            m_name = new string(name);
            cout << "Dog 构造函数调用,m_name" << *m_name << endl;
        }
        ~Dog() {
            if (this->m_name != NULL) {
                delete this->m_name;
                this->m_name = NULL;
            }
            cout << "Dog 析构函数调用" << endl;
        }
    private:
        string *m_name;
};


int main(int argc, char **argv)
{
    Animal *d = new Dog("xiaoming");
    delete d;

    return 0;
}