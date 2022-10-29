// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// /*用C语言编程实现如下字符串变换：“abc defg llkkj”到“cba gfed jkkll”。
// 函数原型: void revers(char *s)
// 参数: s 为输入输出参数
// 返回值: 无
// */

// void revers(char *s)
// {
//     int len = strlen(s);
//     int left_idx = 0;
//     int right_idx = 0;
//     int value = 0;

//     int tmp_left_idx = 0;
//     int tmp_right_idx = 0;
//     int tmp = 0;
//     while(right_idx <= len) {
//         value = s[right_idx];
//         if (value == ' ' || value == '\0') {
//             tmp_left_idx = left_idx;  //"abc defg llkkj";
//             tmp_right_idx = right_idx - 1;

//             while(tmp_left_idx < tmp_right_idx) {
//                 tmp = 0;
//                 tmp = s[tmp_left_idx];
//                 s[tmp_left_idx] = s[tmp_right_idx];
//                 s[tmp_right_idx] = tmp;
//                 tmp_left_idx++;
//                 tmp_right_idx--;
//             }

//             left_idx = right_idx + 1; 
//         }

//         right_idx++;
//     }

// }


// int main(int argc, char **argv) {
//     char *res_str = "abc defg llkkj";
//     char *str = malloc(strlen(res_str) + 1);

//     strncpy(str, res_str, strlen(res_str));

//     revers(str);
//     printf("str is %s\n", str);

//     return 0;
// }











// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// /*
// 先进先出的fifo
// */

// #define MAX_SIZE 110

// typedef struct __fifo_s{
//     int top;    //栈顶
//     int bot;    //栈底
//     int size;   //大小
//     char buf[MAX_SIZE]; //缓冲区
// } FIFO_S;


// // 初始化fifo
// void fifo_init(FIFO_S *fifo){
//     fifo->top=0;
//     fifo->bot=0;
//     fifo->size=0;
// }

// // 数据进入
// char fifo_in(FIFO_S *fifo, char * Addr,int len){
//     int i = 0;
//     //判断FIFO是否还有空间
//     if(len > (MAX_SIZE - fifo->size)) {
//         return 0;
//     }

//     for(i = 0; i < len; i++){
//         fifo->buf[fifo->top] = Addr[i];
//         //计算下一个地址
//         fifo->top = (fifo->top + 1) % MAX_SIZE;
//     }
//     fifo->size += len;
//     return 1;
// }

// // 获取数据
// char fifo_out(FIFO_S *fifo, char* buf, int size){
//     int i = 0;
//     if(fifo->size < size) {
//         return 0;
//     }
//     for(i = 0; i < size; i++){
//         buf[i] = fifo->buf[fifo->bot];
//         fifo->bot = (fifo->bot+1) % MAX_SIZE;
//     }
//     fifo->size -= size;
//     return 1;
// }

// int main()
// {
//     int i;
//     char indata[]={"fifo data"};
//     char outdata[16]={0};
//     FIFO_S fifo = {0};

//     fifo_init(&fifo);
//     //入队测试
//     fifo_in(&fifo, indata, sizeof(indata));
//     //出队测试
//     fifo_out(&fifo, outdata, sizeof(indata));

//     printf("data is:%s\n",outdata);

//     return 0;
// }


















// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// /*
// 用C语言编程对一个字符变量计算其二进制中1的个数,如输入一个5,其二进制为000001001,其中1的个数为2个,则输出2。 
// 函数原型: int count(char c)
// 参数: c 输入
// 返回值: 输入变量中二进制1的个数
// */


// int count(char c) 
// { 
//     int i = 0;
//     int cnt = 0;
//     //char 型  遍历8位
//     for(i = 0; i < 8; i++) {
//         // 按位与，如果该位是0，则结果为0
//         if((c & (1 << i)) != 0) {
//             cnt++;
//         } 
//     }
//     return cnt;
// }

// int main()
// {
//     char value = 68;
//     int cnt = count(value);//

//     printf("cnt is:%d\n",cnt);
//     return 0;
// }





// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #include <math.h>

// /*
// 请编程验证N以内的哥德巴赫猜想.（任意一个大于2的偶数都可以表示成2个素数之和）
// */

// //
// static int __if_prime(int n)
// {
// 	if (n == 1) //1不是素数 
// 		return 0;
		
//     // 开平方，减少for循环次数
// 	int temp = (int)sqrt(n);
// 	for (int i = 2; i <= temp; i++) {
// 		if (n % i == 0) {
// 			return 0;
//         }
//     }
//     // 不能被任何数整除
// 	return 1; 
// }

// void func(int n)
// {
//     int if_prime = 0;
//     int if_prime_another = 0;
//     for(int i = 0; i < n; i++) {
//         if_prime = __if_prime(i);
//         if (if_prime == 1) {
//             for (int j = i + 1; j < n; j++) {
//                 if_prime_another = __if_prime(j);
//                 if (if_prime_another == 1) {
//                     if (i + j == n) {
//                         printf("符合猜想\n");
//                         break;
//                     }
//                 }
//             }
//         }
//     }
// }

// int main()
// {
//     func(5);

//     return 0;


// }

#include <stdio.h>

int main(int argc, char **argv)
{
    int a = -5;//先求原码_>再求补码_>再右移 高位补1->保留符号位，按位取反->再求原码  

    printf("a is %d\n", a << 2);
    printf("a is %d\n", a >> 2);   
}