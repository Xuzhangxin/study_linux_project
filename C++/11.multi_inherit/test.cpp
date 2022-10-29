#include <iostream>

using namespace std;

class Animal {
    public:
        int m_age;
        int m_age1;
        int m_age2;
        int m_age3;
        int m_age4;
        int m_age5;
        int m_age6;
};

class Sheep : virtual public Animal
{};

class Tuo :  virtual public Animal
{};

class SheepTuo : public Tuo, public Sheep {};

int main(int argc, char **argv)
{
    SheepTuo s;
    cout << "size is " << sizeof(s) <<endl;
}