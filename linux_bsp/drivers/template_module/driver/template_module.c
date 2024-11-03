#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple example Linux kernel module");
MODULE_VERSION("1.0");

// 示例模块参数
static int myintparam = 1234;
module_param(myintparam, int, S_IRUGO); // 模块参数声明为只读


void hello_function(void) {
    printk(KERN_INFO "Hello from hello_function!\n");
}
EXPORT_SYMBOL(hello_function); // 导出函数符号

// 模块加载函数
static int __init hello_init(void) {
    printk(KERN_INFO "Hello, kernel! myintparam = %d\n", myintparam);
    return 0; // 如果加载成功，返回0
}

// 模块卸载函数
static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye, kernel!\n");
}

// 模块加载函数
module_init(hello_init);
// 模块卸载函数
module_exit(hello_exit);
