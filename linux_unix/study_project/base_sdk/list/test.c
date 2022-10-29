#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
/*用C语言编程实现如下字符串变换：“abc defg llkkj”到“cba gfed jkkll”。
函数原型: void revers(char *s)
参数: s 为输入输出参数
返回值: 无
*/

void revers(char *s)
{
    int left_idx = 0;
    int right_idx = strlen(s);
    int tmp = 0;
    printf("1111\n");
    while(left_idx < right_idx) {
        printf("2222\n");
        tmp = 0;
        tmp = s[left_idx];
        printf("tmp:%d\n", tmp);
        s[left_idx] = s[right_idx];
        s[right_idx] = tmp;
        left_idx++;
        right_idx--;
    }
}



int main(int argc, char **argv) {
    char *str = "abc defg llkkj";
    revers(str);
    printf("str is %s\n", str);


    return 0;
}