#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

// 选手类
class Person
{
    public:
        Person(string name, int score)
        {
            this->m_name = name;
            this->m_score = score;
        }

    private:
        string m_name;  // 姓名
        int m_score;    // 平均分
};

void createPerson(vector<Person>& v)
{
    string nameSeed = "ABCDE";

    for (int i = 0; i < v.size(); i++) {
        string name = "选手";
        name += nameSeed[i];

        int score = 0;

        Person p(name ,score);
        v.push_back(p);
    }
}


void setScore(vector<Person>& v) {
    for (vector<Person>::iterator it = v.begin(); it != v.end(); it++) {
        deque<int> d;
        for (int i = 0; i < 10; i++ ) {
            int score = rand() % 41 + 60;
            d.push_back(score);
        }

        sort(d.begin(), d.end());
        //去除最高和最低分
        d.pop_back();
        d.pop_front();
    }
}

int main(int argc, char **argv)
{

}