#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>

#define USER_MAJOR 201
#define USER_NAME "user_chrdev"

static int user_open(struct inode *node, struct file *file)
{
    printk("user driver open");
}

static int user_close(struct inode *node, struct file *file)
{
    printk("user driver close");
}

static ssize_t user_read (struct file *file, char __user *data, size_t cnt, loff_t *off)
{
    printk("user driver read");
}
static ssize_t user_write(struct file *file, const char __user *data, size_t cnt, loff_t *off)
{
    printk("user driver write");
}


static struct file_operations user_fops = {
    .owner = THIS_MODULE,
    .open = user_open,
    .release = user_close,
    .read = user_read,
    .write = user_write
};

static int __init user_init(void)
{
    /// 注册驱动
    int ret = register_chrdev(USER_MAJOR, USER_NAME, &user_fops);
    if (ret < 0)
    {
        printk("user driver registration failed");
    }
    printk("user driver init");

    return 0;
}

static int __exit user_exit(void)
{
    /// 注销驱动
    unregister_chrdev(USER_MAJOR, USER_NAME);
    printk("user driver exit");
}


module_init(user_init);
module_exit(user_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuzhangxin");