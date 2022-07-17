#include <iostream>
#include <unistd.h>

using namespace std;

// 菜单界面
void showMenu()
{
    cout << "1、添加联系人" << endl;
    cout << "2、显示联系人" << endl;
    cout << "3、删除联系人" << endl;
    cout << "4、查找联系人" << endl;
    cout << "5、修改联系人" << endl;
    cout << "6、清空联系人" << endl;
    cout << "0、退出" << endl;
}

int main(int argc, char **argv)
{
    int select = 0;

    while(1) {
        // 菜单调用
        showMenu();
        cin >> select;
        switch(select) {
            case 0: //0.退出
                cout << "欢迎下次使用" << endl;
                exit(0);
                break;
            case 1: //1、添加联系人
                break;
            case 2: //2、显示联系人
                break;
            case 3: //3、删除联系人
                break;
            case 4: //4、查找联系人
                break;
            case 5: //5、修改联系人
                break;   
            case 6: //6、清空联系人
                break;           
        }
    }



    return 0;
}