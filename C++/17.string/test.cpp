#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void test_copy()
{
    // 无参构造函数
    string str1;

    // 有参构造函数
    string str2("Hello, world!");
    cout << "str2 is " << str2 << endl;

    // 拷贝构造函数
    string str3(str2);
    cout << "str3 is " << str3 << endl;

    // n个字符C初始化
    string str4(10, 'c');
    cout << "str4 is " << str4 << endl;
}

/* 结果
str1 is h
str1 is he
str1 is hel
str1 is hell
str1 is hello 
str1 is hello wor
str1 is hello world
*/
// 拼接字符串
void test_stitch()
{
    string str1 = "h";
    cout << "str1 is " << str1 << endl;
    str1 += "e";
    cout << "str1 is " << str1 << endl;
    str1 += 'l';
    cout << "str1 is " << str1 << endl;
    str1.append("l");
    cout << "str1 is " << str1 << endl;
    str1.append("o world", 2);
    cout << "str1 is " << str1 << endl;
    string str2 = "wor";
    str1.append(str2);
    cout << "str1 is " << str1 << endl;
    string str3 = "world";
    str1.append(str3, 3, 2);
    cout << "str1 is " << str1 << endl;
}

/* 结果
str1 is hello world
str2 is hello world
str3 is c
str4 is hello world
str5 is hello
str6 is hello world
str7 is cccccccccc
*/
void test_assign()
{
    string str1;
    str1 = "hello world";
    cout << "str1 is " << str1 << endl;

    string str2;
    str2 = str1;
    cout << "str2 is " << str2 << endl;

    string str3;
    str3 = 'c';
    cout << "str3 is " << str3 << endl;

    string str4;
    str4.assign("hello world");
    cout << "str4 is " << str4 << endl;

    string str5;
    str5.assign("hello world", 5);
    cout << "str5 is " << str5 << endl;

    string str6;
    str6.assign(str4);
    cout << "str6 is " << str6 << endl;

    string str7;
    str7.assign(10, 'c');
    cout << "str7 is " << str7 << endl;
}

/* 结果
find pos is 6
rfind pos is 12
replace str1 is haaaaaworld world
*/
void test_find()
{
    string str1 = "hello world world";
    int pos = str1.find("world");
    if (pos == -1) {
        cout << "world is not found" << endl;
        return ;
    }
    cout << "find pos is " << pos << endl;

    pos = str1.rfind("world");
    if (pos == -1) {
        cout << "world is not found" << endl;
        return ;
    }
    cout << "rfind pos is " << pos << endl;

    str1.replace(1, 5, "aaaaa");
    cout << "replace str1 is " << str1 << endl;
}

/* 结果
str1 大于 str2
*/
void test_comp(void) 
{
    string str1 = "hello";
    string str2 = "aello";

    int ret = str1.compare(str2);
    if (ret == 0) {
        cout << "str1 等于 str2";
    } else if (ret > 0) {
        cout << "str1 大于 str2";
    } else if (ret < 0) {
        cout << "str1 小于 str2";
    }
}

/* 结果
str1[4] is o
str1[1] is e
*/
void test_get_char(void)
{
    string str1 = "hello world";
    cout << "str1[4] is "<< str1.at(4) << endl;
    cout << "str1[1] is "<< str1[1]<< endl;
}

/* 结果
str1 is helloaaa world
str1 is hellobbbbaaa world
str1 is hellocccccbbbbaaa world
str1 is hellobbbbaaa world
*/
void test_str_insert(void)
{
    string str1 = "hello world";
    str1.insert(5, "aaa");
    cout << "str1 is " << str1 << endl;

    string str2 = "bbbb";
    str1.insert(5, str2);
    cout << "str1 is " << str1 << endl;

    str1.insert(5, 5, 'c');
    cout << "str1 is " << str1 << endl;

    str1.erase(5, 5);
    cout << "str1 is " << str1 << endl;
}

/* 结果
sub_string is worl
*/
void test_str_child()
{
    string str1 = "hello world";
    string sub_string = str1.substr(6, 4);

    cout << "sub_string is " << sub_string << endl;
}

int main(int argc, char **argv)
{
    //test_copy();
    //test_assign();
    //test_stitch();
    //test_find();
    //test_comp();
    //test_get_char();
    //test_str_insert();
    test_str_child();
   return 0;
}

