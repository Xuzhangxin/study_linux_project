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

#define AP3216C_NAME "ap3216c"

struct ap3216c_dev_t {
    dev_t m_devid;
    struct cdev m_cdev;     ///< 字符设备结构
    struct class *m_class;  ///< 类
    struct device *m_dev;   ///< 设备
    struct device_node *nd; ///< 设备节点
    int devid;              ///< 设备号
    int major;               ///< 主设备号
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

 /* 传统匹配方式 ID 列表 */
static const struct i2c_device_id ap3216c_id[] = {
    {"alientek,ap3216c", 0}, 
    {}
};

static const struct of_device_id ap3216c_of_match[] = {
    {.compatible = "alientek,ap3216c"}
};

int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    printk("ap3216c driver probe begin\n");

    ///1.申请设备号
    if (ap3216c_dev.major) {
        ap3216c_dev.devid = MKDEV(ap3216c_dev.major, 0);
        register_chrdev_region(ap3216c_dev.devid, 1, AP3216C_NAME);
    } else {
        
    }

    ///2.cdev_init、cdev_add

    ///3.class_create

    ///4.device_create    

    printk("ap3216c driver probe done\n");
    return 0;
}

int ap3216c_remove(struct i2c_client *client)
{
    ///cdev_del

    ///注销设备号

    ///注销device和class
    printk("ap3216c driver remove begin\n");

    printk("ap3216c driver remove done\n");
    return 0;
}

static struct i2c_driver ap3216c_driver = {
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "ap3216c",
        .of_match_table = ap3216c_of_match,
    }
    .id_table = ap3216c_id,
};

static int __init ap3216c_init(void)
{
    printk("ap3216c driver init begin\n");

    int ret = i2c_add_driver(&ap3216c_driver);

    printk("ap3216c driver init done, ret:%d\n", ret);

    return ret;
}

static int __exit ap3216c_exit(void)
{
    printk("ap3216c driver exit begin\n"); 

    i2c_del_driver(&ap3216c_driver);

    printk("ap3216c driver exit done\n"); 

    return 0;
}


module_init(ap3216c_init);
module_exit(ap3216c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");