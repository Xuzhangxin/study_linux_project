#include <iostream>
#include <fstream>
#include <string.h>


using namespace std;

int main(int argc, char **argv)
{
    // 输入文件流对象，用于读取
    fstream fs;
    // 写方式打开  （文件流对象和test.txt建立联系）
    fs.open(("./test.txt"), ios::out | ios::binary);
    if (fs.is_open()) {
        cout << "file open successfully" << endl;
    }

    char *str = "hello world";

    fs.write(str, strlen(str));

    fs.close();

    fs.open(("./test.txt"), ios::in | ios::binary);
    if (fs.is_open()) {
        cout << "file open successfully" << endl;
    }

    char buf[1024];
    fs.read((char *)buf, sizeof(buf));

    cout << "buf is" << buf << endl;

    fs.close();
    return 0;
}