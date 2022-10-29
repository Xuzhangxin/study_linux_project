#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
    // 输入文件流对象，用于读取
    ifstream ifs;
    // 写方式打开  （文件流对象和test.txt建立联系）
    ifs.open(("./test.txt"), ios::in);
    if (ifs.is_open()) {
        cout << "file open successfully" << endl;
    }

    // 逐行读取
    char buf[1024] = {0};
    while (ifs >> buf) {
        cout << buf << endl;
    }
    while(ifs.getline(buf, sizeof(buf))) {
        cout << buf << endl;
    }
    ifs.close();
    return 0;
}