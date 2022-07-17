#include <iostream>
using namespace std;

class Person {
    public:
        int m_age;
        string m_name;
        int *p_id;
    // 默认无参构造函数
    Person() {
        m_age = 10;
        m_name = "lisi";
        p_id = new int(5);
    }
    // 默认有参构造函数
    Person(int age, string name) {
        m_age = age;
        m_name = name;
        p_id = new int(5);
    }
    // 拷贝构造函数
    Person(const Person & p) {
        m_age = p.m_age;
        m_name = p.m_name;
        // p_id = p.p_id;  //note:double free 操作的是同一片内存
        p_id = new int(*p.p_id);
    }
    // 析构函数
    ~Person() {
        if (NULL != p_id) {
            delete p_id;
            p_id = NULL;
        }
    }
};


int main(int argc, char **argv)
{
    Person p1;
    cout << "1)age:" << p1.m_age << "  name" << p1.m_name << endl;
    Person p2(20, "zhangsan");
    cout << "2)age:" << p2.m_age << "  name" << p2.m_name << endl;
    Person p3(p2);
    cout << "3)age:" << p3.m_age << "  name" << p3.m_name << endl;

    return 0;
}