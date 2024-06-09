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
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

struct ap3216c_dev_t {
    dev_t m_devid;
    struct cdev m_cdev;     ///< 字符设备结构
    struct class *m_class;  ///< 类
    struct device *m_dev;   ///< 设备
    struct device_node *nd; /* 设备节点 */
    int major               ///< 主设备号
};

struct ap3216c_dev_t ap3216c_dev = {0}; 

static int ap3216c_open(struct inode *node, struct file *file)
{
    file->private_data = (void *)&ap3216c_dev; ///< 设置私有数据
    printk("ap3216c driver open\n");

    return 0;
}

static int ap3216c_close(struct inode *node, struct file *file)
{
    printk("ap3216c driver close\n");

    return 0;
}

static ssize_t ap3216c_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    printk("ap3216c driver read\n");

    printk("ap3216c driver read sucess, ret:%d\n", ret);
    return ret;
}
static ssize_t ap3216c_write(struct file *file, const char __user *data, size_t cnt, loff_t *off)
{
    printk("ap3216c driver write\n");


    printk("ap3216c driver write sucess, buf[0]:%d", private_dev->m_buf[0]);
    return ret;
}

static struct file_operations ap3216c_fops = {
    .owner = THIS_MODULE,
    .open = ap3216c_open,
    .release = ap3216c_close,
    .read = ap3216c_read,
    .write = ap3216c_write
};




static struct i2c_driver ap3216c_driver = {
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    
} 


static int __init ap3216c_init(void)
{
    int ret = i2c_add_driver();
    printk("ap3216c driver init\n");

    return 0;
}

static int __exit ap3216c_exit(void)
{
    int
    printk("ap3216c driver exit\n"); 
    return 0;
}


module_init(ap3216c_init);
module_exit(ap3216c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");