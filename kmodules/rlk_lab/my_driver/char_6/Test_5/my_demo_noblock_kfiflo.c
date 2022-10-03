#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kfifo.h>

#define DEMO_NAME "my_demo_dev_kfifo_noblock"

#define MAX_DEVICE_BUFFER_SIZE 64
DEFINE_KFIFO(mydemo_kfifo,char,MAX_DEVICE_BUFFER_SIZE);


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
    int ret;

    printk("%s enter,ppos = %s\n",__func__,*ppos);

    if(kfifo_is_empty(&mydemo_kfifo)) {
        if(file->f_flags & O_NONBLOCK)
            return -EAGAIN;
    }


   // ret = copy_to_user(buf,device_buffer + *ppos,need_read);
   ret = kfifo_to_user(&mydemo_kfifo,buf,lbuf,&actual_readed);

    if(ret)
        return -EIO;

    printk("%s,actual_readed = %d,pos = %d\n",__func__,actual_readed,*ppos);

    return actual_readed;
}
//	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops)
{
    int actual_write;
    int ret;

    printk("%s enter fops = %s\n",__func__,*f_ops);
    if(kfifo_is_full(&mydemo_kfifo)) {
        if(file->f_flags & O_NONBLOCK)
            return -EAGAIN;
       }

    // ret = copy_from_user(device_buffer + *f_ops, buf, need_write);
    ret = kfifo_from_user(&mydemo_kfifo,buf,count,&actual_write);
    if(ret)
        return -EIO;

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

    ret = misc_register(&mydemodrv_misc_device);
    if(ret) {
        printk("failed register misc device\n");
        return ret;
    }


    printk("succeeded register misc device: %s\n",DEMO_NAME);

    return 0;
}

static void __exit simple_char_exit(void)
{
    printk("removing device\n");
    misc_deregister(&mydemodrv_misc_device);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("silence");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simple character device");