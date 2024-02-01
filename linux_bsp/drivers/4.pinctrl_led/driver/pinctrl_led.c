#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR 202
#define LED_NAME "pinctrl_led"

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


struct led_dev_t {
    dev_t m_devid;
    struct cdev m_cdev;     ///< 字符设备结构
    struct class *m_class;  ///< 类
    struct device *m_dev;   ///< 设备
    struct device_node *nd; /* 设备节点 */
    unsigned char m_buf[1024];
    int led_gpio;           ///< led 对应的gpio编号 
};

struct led_dev_t led_dev = {0}; 

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
    printk("pinctrl_led driver open\n");

    return 0;
}

static int led_close(struct inode *node, struct file *file)
{
    printk("pinctrl_led driver close\n");

    return 0;
}

static ssize_t led_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    printk("pinctrl_led driver read\n");
    struct led_dev_t *private_dev = (struct led_dev_t *)file->private_data;

    int ret = 0;
    ret = copy_to_user(data, private_dev->m_buf, cnt);
    if (ret < 0)
    {
        printk("pinctrl_led driver read failed:%d", ret);
        return ret;
    }

    printk("pinctrl_led driver read sucess, ret:%d\n", ret);
    return ret;
}
static ssize_t led_write(struct file *file, const char __user *data, size_t cnt, loff_t *off)
{
    printk("pinctrl_led driver write\n");

    struct led_dev_t *private_dev = (struct led_dev_t *)file->private_data;

    int ret = 0;
    ret = copy_from_user(private_dev->m_buf, data, cnt);
    if (ret < 0) {
        printk("pinctrl_led driver write failed:%d\n", ret);
        return -EFAULT;
    }

    if (private_dev->m_buf[0] == LED_ON) {
        gpio_set_value(private_dev->led_gpio, LED_ON);
    } else if (private_dev->m_buf[0] == LED_OFF) {
        gpio_set_value(private_dev->led_gpio, LED_OFF);
    } else {
        printk("pinctrl_led driver write failed, buf[0]:%d\n", private_dev->m_buf[0]);
        return -EFAULT;
    }

    printk("pinctrl_led driver write sucess, buf[0]:%d", private_dev->m_buf[0]);
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
    int ret = 0;
    uint32_t val = 0;
    int major = -1;
    int minor = -1;

    u32 regdata[14];

    /// 初始化cdev
    cdev_init(&led_dev.m_cdev, &led_fops);

    /// 1.获取设备树节点
    led_dev.nd = of_find_node_by_path("/gpio_myled");
    if (!led_dev.nd)
    {
        printk("gpio_myled node not found\n");
        return -EINVAL;
    }

    /// 2.获取设备树中的gpio属性，得到led编号
    led_dev.led_gpio = of_get_named_gpio(led_dev.nd, "led-gpio", 0);
    if (led_dev.led_gpio < 0) {
        printk("led-gpio node not found\n");
        return -EINVAL;
    }

    /// 3.设置gpio为输出，输出高电平，关led
    ret = gpio_direction_output(led_dev.led_gpio, 1);
    if (ret < 0) {
        printk("pinctrl led set output failed\n");
        return -EINVAL;
    }

    /// 4.申请设备号
    if (LED_MAJOR)
    {
        led_dev.m_devid = MKDEV(LED_MAJOR, 0);
        register_chrdev_region(led_dev.m_devid, 1, LED_NAME);  /// 给定设备号
        printk("pinctrl led major:%d, minor:%d", LED_MAJOR, 0);
    }
    else
    {
        alloc_chrdev_region(&led_dev.m_devid, 0, 1, LED_NAME);  /// 申请设备号
        major = MAJOR(led_dev.m_devid);
        minor = MINOR(led_dev.m_devid);
        printk("pinctrl led major:%d, minor:%d", major, minor);
    }


    /// 4.注册设备
    led_dev.m_cdev.owner = THIS_MODULE;
    cdev_add(&led_dev.m_cdev, led_dev.m_devid, 1);

    /// 5.添加自动创建设备节点
    led_dev.m_class = class_create(THIS_MODULE, LED_NAME);  ///< 创建类
    led_dev.m_dev = device_create(led_dev.m_class, NULL, led_dev.m_devid, NULL, LED_NAME);///< 创建设备

    printk("pinctrl_led driver init\n");

    return 0;
}

static int __exit led_exit(void)
{
    /// 注销字符设备
    unregister_chrdev_region(led_dev.m_devid, 1); ///< 释放设备号
    cdev_del(&led_dev.m_cdev);  ///< 删除设备
    device_destroy(led_dev.m_class, led_dev.m_devid); ///< 销毁设备
    class_destroy(led_dev.m_class); ///< 销毁类

    printk("pinctrl_led driver exit\n"); 
    return 0;
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");