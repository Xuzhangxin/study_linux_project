#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR 201
#define LED_NAME "led_dev"

#define LED_OFF 0
#define LED_ON 1
 
/// 寄存器物理地址
#define CCM_CCGR1_BASE (0X020C406C) 
#define SW_MUX_GPIO1_IO03_BASE (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE (0X020E02F4)
#define GPIO1_DR_BASE (0X0209C000)
#define GPIO1_GDIR_BASE (0X0209C004)

static void __iomem *IMX_CCM_CCGR1;     /// 时钟使能寄存器
static void __iomem *SW_MUX_GPIO1_IO03; /// GPIO1  IO复用
static void __iomem *SW_PAD_GPIO1_IO03; /// GPIO1 IO属性
static void __iomem *GPIO1_DR;          /// GPIO1 数据寄存器
static void __iomem *GPIO1_GDIR;        /// GPIO1 输出方向


void led_ctrl(u8 status)
{
    u32 val = 0;
    if (status == LED_OFF) {
        val = readl(GPIO1_DR);
        val &= ~(1 << 3); 
        writel(val, GPIO1_DR);
    } else if (status == LED_ON) {
        val = readl(GPIO1_DR);
        val |= (1 << 3); 
        writel(val, GPIO1_DR);
    }
}

static int led_open(struct inode *node, struct file *file)
{
    file->private_data = (void *)&led_dev; ///< 设置私有数据
    printk("user led driver open\n");

    return 0;
}

static int led_close(struct inode *node, struct file *file)
{
    printk("user led driver close\n");
}

static ssize_t led_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    printk("user led driver read\n");

    int ret = 0;
    ret = copy_to_user(data, file->private_data->m_buf, cnt);
    if (ret < 0)
    {
        printk("user led driver read failed:%d", ret);
        return ret;
    }

    printk("user led driver read sucess, ret:%d\n", ret);
    return ret;
}
static ssize_t led_write(struct file *file, const char __user *data, size_t cnt, loff_t *off)
{
    printk("user led driver write\n");

    int ret = 0;
    ret = copy_from_user(file->private_data->m_buf, data, cnt);
    if (ret < 0) {
        printk("user led driver write failed:%d\n", ret);
        return -EFAULT;
    }

    if (file->private_data->m_buf[0] == LED_ON) {
        led_ctrl(LED_ON);
    } else if (file->private_data->m_buf[0] == LED_OFF) {
        led_ctrl(LED_OFF);
    } else {
        printk("user led driver write failed, buf[0]:%d\n", file->private_data->m_buf[0]);
        return -EFAULT;
    }

    printk("user led driver write sucess, buf[0]:%d", file->private_data->m_buf[0]);
    return ret;
}

struct led_dev_t {
    dev_t m_devid;
    struct cdev m_cdev;     ///< 字符设备结构
    struct class *m_class;  ///< 类
    struct device *m_dev;   ///< 设备
    unsigned char m_buf[1024];
};

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close,
    .read = led_read,
    .write = led_write
};

struct led_dev_t led_dev = {0}; 

static int __init led_init(void)
{
    int ret = 0;
    uint32_t val = 0;
    int major = -1;
    int minor = -1;

    /// 初始化cdev
    cdev_init(&led_dev.m_cdev, &led_fops);

    /// 寄存器地址映射
    IMX_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
    SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
    GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

    /// 使能GPIO1时钟
    val = readl(IMX_CCM_CCGR1);
    val &= (3 << 26); /// 结合寄存器手册，清除之前的时钟使能寄存器值
    val |= (3 << 26);
    writel(val, IMX_CCM_CCGR1);

    /// 设置IO复用
    writel(5, SW_MUX_GPIO1_IO03);

    /// 设置IO属性
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    /// 设置IO为输出
    val = readl(GPIO1_GDIR);
    val &= ~(1 << 3);
    val |= (1 << 3); 
    writel(val, GPIO1_GDIR);

    /// 关闭LED
    val = readl(GPIO1_DR);
    val |= (1 << 3); 
    writel(val, GPIO1_DR);

    /// 申请设备号
    if (LED_MAJOR)
    {
        led_dev.m_devid = MKDEV(LED_MAJOR, 0);
        register_chrdev_region(led_dev.m_devid, 1, LED_NAME);  /// 给定设备号
    }
    else
    {
        alloc_chrdev_region(&led_dev.m_devid, 0, 1, LED_NAME);  /// 申请设备号
        major = MAJOR(led_dev.m_devid);
        minor = MINOR(led_dev.m_devid);
    }

    /// 注册设备
    led_dev.m_cdev.owner = THIS_MODULE;
    cdev_add(&led_dev.m_cdev, led_dev.m_devid, 1);

    /// 添加自动创建设备节点
    led_dev.m_class = class_create(THIS_MODULE, LED_NAME);  ///< 创建类
    led_dev.m_dev = device_create(led_dev.m_class, NULL, led_dev.m_devid, NULL, LED_NAME);///< 创建设备

    printk("user led driver init\n");

    return 0;
}

static int __exit led_exit(void)
{
    /// 取消映射
    iounmap(IMX_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);

    /// 注销字符设备
    unregister_chrdev_region(led_dev.m_devid, 1); ///< 释放设备号
    cdev_del(&led_dev.m_cdev);  ///< 删除设备
    device_destroy(led_dev.m_class, led_dev.m_devid); ///< 销毁设备
    class_destroy(led_dev.m_class); ///< 销毁类

    printk("user led driver exit\n"); 
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");