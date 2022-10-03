#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEMO_NAME "my_demo_dev_fifo"
#define MIN(a,b) ((a)<(b) ? (a):(b))

static struct device *mydemodrv_device;
// static char kernel_buf[1024];
static char *device_buffer;
#define MAX_DEVICE_BUFFER_SIZE 64


static int demodrv_open(struct inode *inode, struct file *file)
{
    int major = MAJOR(inode->i_rdev);
    int minor = MINOR(inode->i_rdev);

    printk("%s:major = %d,minor = %d\n",__func__,major,minor);

    return 0;
}

//int (*release) (struct inode *, struct file *);
static int demodrv_release(struct inode *inode, struct file *file)
{
    return 0;
}

//	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);

static ssize_t
demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
    int actual_readed;
    int max_free;
    int need_read;
    int ret;

    printk("%s enter,ppos = %s\n",__func__,*ppos);

    max_free = MAX_DEVICE_BUFFER_SIZE - *ppos;
    need_read = max_free > lbuf ? lbuf : max_free;
    if(need_read == 0)
        dev_warn(mydemodrv_device, "no space for read\n");

    ret = copy_to_user(buf,device_buffer + *ppos,need_read);
    if(ret == need_read)
        return -EFAULT;

    actual_readed = need_read - ret;
    *ppos += actual_readed;

    printk("%s,actual_readed = %d,pos = %d\n",__func__,actual_readed,*ppos);

    return actual_readed;
}
//	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops)
{
    int actual_write;
    int free;
    int need_write;
    int ret;

    printk("%s enter fops = %s\n",__func__,*f_ops);
    free = MAX_DEVICE_BUFFER_SIZE - *f_ops;
    need_write = free > count ? count : free;
    if(need_write == 0)
    {
        dev_warn(mydemodrv_device,"no space for write\n");
    }

    ret = copy_from_user(device_buffer + *f_ops, buf, need_write);
    if(ret == need_write)
        return -EFAULT;

    actual_write = need_write - ret;
    *f_ops += actual_write;

    printk("%s:actual_write = %d,ppos=%d\n",__func__,actual_write,*f_ops);
    return actual_write;
}


static const struct file_operations demodrv_fops = {
    .owner = THIS_MODULE,
    .open = demodrv_open,
    .release = demodrv_release,
    .read = demodrv_read,
    .write = demodrv_write
};

static struct miscdevice mydemodrv_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEMO_NAME,
    .fops = &demodrv_fops,
};

static int __init simple_char_init(void)
{
    int ret;
    //ret = alloc_chrdev_region(&dev,0,count,DEMO_NAME);

    device_buffer = kmalloc(MAX_DEVICE_BUFFER_SIZE, GFP_KERNEL);
    if(!device_buffer)
        return -ENOMEM;
    ret = misc_register(&mydemodrv_misc_device);
    if(ret) {
        printk("failed register misc device\n");
        kfree(device_buffer);
        return ret;
    }

    mydemodrv_device = mydemodrv_misc_device.this_device;

    printk("succeeded register misc device: %s\n",DEMO_NAME);

    return 0;
}

static void __exit simple_char_exit(void)
{
    printk("removing device\n");
    kfree(device_buffer);
    misc_deregister(&mydemodrv_misc_device);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("silence");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simple character device");