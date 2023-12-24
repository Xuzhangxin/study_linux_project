#include <iostream>
using namespace std;

class salesData
{
private:
    
public:
    salesData();
    ~salesData();

    /// 二元操作符，操作两个对象
    friend ostream& operator<< (ostream& output, const salesData& data);        ///<友元函数，用于访问类的私有成员
    friend istream& operator>> (istream& input,  salesData& data);

    friend salesData operator+ (const salesData &data1, const salesData &data2);
    friend salesData& operator- (const salesData &data1, const salesData &data2);

private:
    int m_iHeight;
    int m_iWidth;
};

salesData::salesData()
{
    m_iWidth = 640;
    m_iHeight = 480;
}

salesData::~salesData()
{

}

/// 重载输出运算符
/// std::cout << "salesData" << "1" << std::endl;
ostream& operator<< (ostream& output, const salesData& data)
{
    // output << "witdth" << data.m_iWidth << "height" << data.m_iHeight;
    output << "witdth:" << data.m_iWidth  << "height:" << data.m_iHeight;

    return output;
}

istream& operator>> (istream& input,  salesData& data)
{
    input >> data.m_iWidth >> data.m_iHeight;

    return input;
}

salesData operator+ (const salesData &data1, const salesData &data2)
{
    salesData out;
    out.m_iWidth = data1.m_iWidth + data2.m_iWidth;
    out.m_iHeight = data1.m_iHeight + data2.m_iHeight;
    return out;
}

int main(int argc, char **argv)
{
    salesData data1;

    cout << "data1:" << data1 << endl;

    cin >> data1;

    cout << "data1:" << data1 << endl;

    salesData data2;
    cout << data1 + data2 << endl;

    // cout << "hello" << endl;

    return 0;
}
