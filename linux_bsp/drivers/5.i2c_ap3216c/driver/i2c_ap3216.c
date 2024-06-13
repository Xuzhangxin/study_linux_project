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

/* AP3316C 寄存器 */
#define AP3216C_SYSTEMCONG 0x00 /* 配置寄存器 */
#define AP3216C_INTSTATUS 0X01 /* 中断状态寄存器 */
#define AP3216C_INTCLEAR 0X02 /* 中断清除寄存器 */
#define AP3216C_IRDATALOW 0x0A /* IR 数据低字节 */
#define AP3216C_IRDATAHIGH 0x0B /* IR 数据高字节 */
#define AP3216C_ALSDATALOW 0x0C /* ALS 数据低字节 */
#define AP3216C_ALSDATAHIGH 0X0D /* ALS 数据高字节 */
#define AP3216C_PSDATALOW 0X0E /* PS 数据低字节 */
#define AP3216C_PSDATAHIGH 0X0F /* PS 数据高字节 */

#define AP3216C_DATA_REG_NUM 6

#define AP3216C_NAME "ap3216c"

struct ap3216c_dev_t {
    dev_t m_devid;
    struct cdev m_cdev;     ///< 字符设备结构
    struct class *m_class;  ///< 类
    struct device *m_dev;   ///< 设备
    struct device_node *nd; ///< 设备节点
    int devid;              ///< 设备号
    int major;               ///< 主设备号
    struct i2c_client* client; ///<i2c设备
} ap3216c_dev;

struct ap3216c_dev_t ap3216c_dev = {0}; 

static int ap3216c_write_reg(struct ap3216c_dev *dev, u8 reg, u8 *data, u16 len)
{
    int ret = 0;
    struct i2c_client* client = (i2c_client*)dev->client;
    struct i2c_msg msg;

    msg.addr = client->addr;            ///< 设备地址
    msg.flags = 0;                      ///< 写数据
    msg.buf[0] = reg;                   ///< 操作寄存器地址
    memcpy(&msg.buf[1], data, len);     ///< 写寄存器的数据
    msg.len = len + 1 ;                 ///< 1byte寄存器地址+data的长度    
    return ret = i2c_transfer(client->adapter, &msg, 1);
}

static int ap3216c_read_reg(struct ap3216c_dev *dev, u8 reg, u8 *data, u16 len)
{
    int ret = 0;
    struct i2c_client* client = (i2c_client*)dev->client;
    struct i2c_msg msg[2];

    msg[0].addr = client->addr;             ///< 设备地址
    msg[0].flags = 0;                       ///< 写数据
    msg[0].buf[0] = reg;                    ///< 操作寄存器地址
    msg[0].len = 1 ;                        ///< 1byte寄存器地址 

    msg[1].addr = client->addr;            ///< 设备地址
    msg[1].flags = I2C_M_RD;               ///< 读数据
    msg[1].buf = data;                     ///< 读取数据缓冲区
    msg[1].len = len;                      ///< 读取数据长度 

    return ret = i2c_transfer(client->adapter, &msg, 2);
}


static int ap3216c_open(struct inode *node, struct file *file)
{
    printk("ap3216c driver open\n");
    file->private_data = (void *)&ap3216c_dev; ///< 设置私有数据
    
    u8 value = 0x00;
    ap3216c_write_reg(file->private_data, AP3216C_SYSTEMCONG, &value, 1);  ///< set sw reset
    udelay(20 * 1000); ///< 最少等待10ms
    value = 0x03;
    ap3216c_write_reg(file->private_data, AP3216C_SYSTEMCONG, &value, 1);  ///< 设置为ALS and PS+IR模式

    printk("ap3216c driver open success\n");
    return 0;
}

static int ap3216c_close(struct inode *node, struct file *file)
{
    printk("ap3216c driver close\n");
    file->private_data = (void *)&ap3216c_dev; ///< 设置私有数据
    
    u8 value = 0x00;
    ap3216c_write_reg(file->private_data, AP3216C_SYSTEMCONG, &value, 1);  ///< set sw reset
    
    printk("ap3216c driver close success\n");
    return 0;
}

static ssize_t ap3216c_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    u8 i = 0;
    int ret = 0;
    printk("ap3216c driver read\n");

    file->private_data = (void *)&ap3216c_dev; ///< 设置私有数据

    u8 buf[AP3216C_DATA_REG_NUM] = {0};
    /// 循环将IR、ALS、PS中的寄存器数据读取
    for (i = 0; i < AP3216C_DATA_REG_NUM; i++) {
        ret = ap3216c_read_reg(file->private_data, AP3216C_IRDATALOW, buf, 6);
        printk("ap3216c driver read ret:%d, reg:%x, value:%d\n", ret, AP3216C_IRDATALOW + i, buf[i]);
    }

    printk("ap3216c driver read sucess, ret:%d\n", ret);
    return ret;
}

static struct file_operations ap3216c_fops = {
    .owner = THIS_MODULE,
    .open = ap3216c_open,
    .release = ap3216c_close,
    .read = ap3216c_read,
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
        alloc_chrdev_region(&ap3216c_dev.devid, 0, 1, AP3216C_NAME);
        ap3216c_dev.major = MAJOR(ap3216c_dev.devid);
    }

    ///2.cdev_init、cdev_add
    cdev_init(&ap3216c_dev.m_cdev, &ap3216c_fops);
    cdev_add(&ap3216c_dev.m_cdev, ap3216c_dev.devid, 1);

    ///3.class_create
    ap3216c_dev.m_class = class_create(THIS_MODULE, AP3216C_NAME);
    if (IS_ERR(ap3216c_dev.m_class)) {
        return PTR_ERR(ap3216cdev.class);
    }

    ///4.device_create    
    ap3216c_dev.m_dev = device_create(ap3216c_dev.m_class, NULL, ap3216c_dev.devid, NULL, AP3216C_NAME);
    if (IS_ERR(ap3216c_dev.m_dev)) {
        return PTR_ERR(ap3216cdev.m_dev);
    }

    printk("ap3216c driver probe done\n");
    return 0;
}

int ap3216c_remove(struct i2c_client *client)
{
    printk("ap3216c driver remove begin\n");
    ///cdev_del
    cdev_del(&ap3216c_dev.m_cdev);

    ///注销设备号
    unregister_chrdev(ap3216c_dev.devid, AP3216C_NAME);

    ///device_destroy
    device_destroy(ap3216c_dev.m_class, ap3216c_dev.devid);
    
    ///class_desroy
    class_destroy(ap3216c_dev.m_class);
    
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
    },
    .id_table = ap3216c_id,
};

static int __init ap3216c_init(void)
{
    printk("ap3216c driver init begin\n");

    int ret = 0;
    
    ret = i2c_add_driver(&ap3216c_driver);

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