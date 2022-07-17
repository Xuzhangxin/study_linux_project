#include <iostream>
using namespace std;



int main(int argc, char **argv)
{
    // 堆区申请内存   初始值20
    int *data = new int(20);

    cout << *data <<endl;

    delete data; // 释放内存

    int *arr = new int[10];// 数组有10个元素,已初始化好
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
        cout << arr[i] << "is" << endl;
    }
    // 释放数组加中括号
    delete[] arr;
}