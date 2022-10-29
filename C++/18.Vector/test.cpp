#include <iostream>
#include <vector>

using namespace std;

void print_vector(const vector<int>& v)
{
    for (vector<int>::const_iterator i = v.begin(); i != v.end(); i++) {
        cout << "v is " << *i << endl;
    }
}

int main(int argc, char **argv)
{
    vector<int> v1;

    for (int i = 0; i < 5; i++) {
        v1.push_back(i);
    }
    
    print_vector(v1);

    /// 拷贝构造
    vector<int> v2(v1);
    print_vector(v2);

    /// 区间拷贝
    vector<int> v3(v1.begin(), v1.end());
    print_vector(v3);

    /// 多拷贝
    vector<int> v4(5, 1);
    print_vector(v4);

    return 0;
}