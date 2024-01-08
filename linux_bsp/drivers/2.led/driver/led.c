#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR 201
#define LED_NAME "led_dev"

#define LEDOFF 0 /* 关灯 */
#define LEDON 1 /* 开灯 */
 
/// 寄存器物理地址
#define CCM_CCGR1_BASE (0X020C406C) 
#define SW_MUX_GPIO1_IO03_BASE (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE (0X020E02F4)
#define GPIO1_DR_BASE (0X0209C000)
#define GPIO1_GDIR_BASE (0X0209C004)

char pri_buf[1024] = {0};

static int led_open(struct inode *node, struct file *file)
{
    printk("user led driver open");
}

static int led_close(struct inode *node, struct file *file)
{
    printk("user led driver close");
}

static ssize_t led_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    printk("user led driver read");

    int ret = 0;
    ret = copy_to_user(data, pri_buf, cnt);
    if (ret < 0)
    {
        printk("user led driver read failed:%d", ret);
        return ret;
    }

    printk("user led driver read sucess, ret:%d", ret);
    return ret;
}
static ssize_t led_write(struct file *file, const char __user *data, size_t cnt, loff_t *off)
{
    printk("user led driver write");

    int ret = 0;
    ret = copy_from_user(pri_buf, data, cnt);
    if (ret < 0)
    {
        printk("user led driver write failed:%d", ret);
        return ret;
    }

    printk("user led driver write sucess, ret:%d", ret);
    return ret;
}


static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close,
    .read = led_read,
    .write = led_write
};

static int __init led_init(void)
{
    /// 注册驱动
    int ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (ret < 0)
    {
        printk("user led driver registration failed");
    }
    printk("user led driver init");

    return 0;
}

static int __exit led_exit(void)
{
    /// 注销驱动
    unregister_chrdev(LED_MAJOR, LED_NAME);
    printk("user led driver exit");
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");