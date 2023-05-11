#include <functional>
#include <iostream>
#include <memory>
using namespace std;
using namespace placeholders;

void test_func(int a)
{
    cout << "a is " << a << endl;
}

void test_func2(int a, int b)
{
    cout << "a is " << a << " b is " << b << endl;
}

class TEST_T
{
    public:
        int func(int a, int b);
};

int TEST_T::func(int a, int b)
{
    cout << "a is " << a << " b is " << b << endl;
    return 0;
}

int main(int argc, char** argv)
{
    /// 绑定普通函数
    auto test_func5 = std::bind(test_func, 5);
    test_func5();

    auto test_func6 = std::bind(test_func2, _1, 1);
    test_func6(1);

    /// 绑定类成员函数
    TEST_T obj_test;
    auto test_func7 = std::bind(&TEST_T::func, &obj_test, 2, _1);
    test_func7(5);


    // function存储自由函数,
    std::function<void(int)> fun_std_1 = test_func;    
    fun_std_1(-9);

    /// 存储std_bind调用的结果
    std::function<void(int)> fun_std_2 = std::bind(test_func2, _1, 1);
    fun_std_2(2);

    /// lambda表达式示例
    int a = 0;
    auto f1 = [=]()->int{return a;};
    cout << "lambda a is " << f1() << endl;

    auto f2 = [&]()->int{a++;};
    f2();
    cout << "lambda a is " << a << endl;

    return 0;
}