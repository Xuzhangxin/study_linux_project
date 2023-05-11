#include <iostream>
#include <map>
#include <functional>
using namespace std;
using namespace placeholders;

class TEST_T
{
    public:
    template <class T>
    int func(T t);
    int func_test1(int a);
    int func_test2(int b);
};

template <class T>
int TEST_T::func(T t)
{
    bool if_int = is_convertible<T, int>::value;
    bool if_string = is_convertible<T, string>::value;
    cout << "if_int: " << if_int << ", if_string: " << if_string << endl;
}

int TEST_T::func_test1(int a)
{
    cout << "func_test1 a: " << a << endl;
}

int TEST_T::func_test2(int a)
{
    cout << "func_test2 a: " << a << endl;
}

template <class T>
int setParaConfigValue(T value)
{
    if (is_same<T, string>::value) {  //is_convertible
        printf("string");
    } else if(is_same<T,int>::value || std::is_same<T, bool>::value)
    {
        printf("int");
    }
}

typedef std::function<int(int)> map_function;
TEST_T obj_test;
map<string, map_function> g_map;

//

int main(int argc,  char **argv)
{

    // TEST_T t;
    // bool value = 0;
    // string value_str("test");
    // char *str ="value";
    // // t.func(str);

    // setParaConfigValue(value);
    map_function func= std::bind(&TEST_T::func_test1, &obj_test, _1);
    g_map.insert(make_pair<string, map_function>("1",std::bind(&TEST_T::func_test1, &obj_test, _1)));
    g_map.insert(make_pair<string, map_function>("2",std::bind(&TEST_T::func_test2, &obj_test, _1)));

    g_map["1"](5);
    g_map["2"](3);
    return 0;
}