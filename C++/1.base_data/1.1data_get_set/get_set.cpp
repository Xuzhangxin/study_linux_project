#include <iostream>
#include <bitset>

using namespace std;


int main(int argc, char **argv)
{
    short data = -10;
    cout << bitset<16> (data) << endl;
}
