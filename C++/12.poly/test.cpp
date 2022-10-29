#include <iostream>
using namespace std;

class Animal {
    public:
        virtual void speak(void) {
            cout << "hello, I am animal" << endl;
        }
};

class Dog : public Animal {
    public:
        // void speak(void) {
        //     cout << "hello, I am dog" << endl;
        // }
};

class Cat : public Animal {
    public:
        void speak(void) {
            cout << "hello, I am cat" << endl;
        }
};

void speak_now(Animal & animal) 
{
    animal.speak();
}

int main(int argc, char **argv)
{
    Cat c;
    //Cat::Animal::speak(); // 若为静态成员，共享同一份内存数据，可通过类名访问

    Dog d;

    // speak_now(c);
    // speak_now(d);

    return 0;
}