#include <iostream>
#include <string.h>
////a:196, b:-60, c:196, d:65476, dd:4
//   始终要牢记内存中的表示，至于printf或者强制类型转换都只不过是对数的不同解释。
//   如将有符号低精度数转换为高精度时，符号位向上扩展，如果是无符号低精度转换为高精度时，高位补0即可。
//   如下程序所示。与此对应的是，如果是高精度转换为低精度时，那么只会装载高精度的相应低位，无论该数为signed 或是unsigned。
int main(int argc, char **argv)
{

    //1100 0100
    uint16_t a = 0x00C4;
    uint16_t aa = 0x0004;
    char b = (char)a;
    char bb = (char)aa;
    uint8_t c = (uint8_t)b;
    uint16_t d = (uint16_t)b;
    uint16_t dd = (uint16_t)bb;

    printf("a:%u, b:%d, c:%u, d:%u, dd:%u\n", a, b, c, d, dd);

    return 0;
}