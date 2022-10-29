#include <iostream>
#include <vector>
#include<algorithm>
using namespace std;

int test(void)
{
	vector<string> strVector;
	strVector.reserve(5);//当前空间够大，不会发生重新配置，插入新元素后有可能会重新分配
 
	strVector.push_back("hello");
	strVector.push_back("C++");
	strVector.push_back("world");
 
	vector<string>::iterator it = strVector.begin();
	cout << "chang size befor, the first elemt:" << *it << endl;
 
	cout << "push_back one elems:." << endl;
	strVector.push_back("MS");
	cout << "push_back one elemt after, the first elemt:" << *it << endl;
 
	cout << "push_back two elems:" << endl;
	strVector.push_back("HW");
    strVector.push_back("BAT");//当前大小超过当前的容量，导致重新分配内存
	//it = strVector.begin(); 内存重新分配后，重新获取指针可以避免指针失效
	cout << "push_back two elemts after, the first elemt:"<< *it << endl;//指针失效导致程序奔溃，

	return 0;
}

int main(int argc, char **argv)
{
    vector<int> v;

    for(int i = 0; i < 100; i++) {
        v.push_back(i);
    }
    // capacity会根据不同策略，设置大小，典型的以空间换时间。
    cout << "v的容量为" << v.capacity() << endl;
    cout << "v的大小为" << v.size() << endl;

    v.resize(3);
    v.insert(v.begin() + 1, 10);
    cout << "v的容量为" << v.capacity() << endl;
    cout << "v的大小为" << v.size() << endl;

    //收缩内存，占用的capacity变小
    vector<int>(v).swap(v);

    cout << "v的容量为" << v.capacity() << endl;
    cout << "v的大小为" << v.size() << endl;

    return 0;
}