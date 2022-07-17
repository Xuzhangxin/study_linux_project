#include <iostream>
#include <string>

using namespace std;    //命名空间 引用C++标准库的作用

class Student {
    public:
        void setName(string name) {
            m_name = name;
        }
        void showName() { 
            cout << "Student name:" << m_name << endl;
        }

    private:
        int m_age;
        string m_name;
};

int main(int argc, char **argv)
{
    Student me;
    me.setName("12345");
    me.showName();

    return 0;
}