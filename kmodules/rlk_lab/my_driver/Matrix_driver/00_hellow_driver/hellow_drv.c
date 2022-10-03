// 参考 driver/char 中的程序，包含头文件，写框架，传输数据:
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>


// A. 驱动中实现 open, read, write, release，APP 调用这些函数时，都打印内核信息 B. APP 调用 write 函数时，传入的数据保存在驱动中
// C. APP 调用 read 函数时，把驱动中保存的数据返回给 APP

//1 确定主设备号，也可以让内核分配
static int major = 0;
static struct cdev cdev_hello;
static char kernel_buf[1024];

#define MIN(a,b) (a < b ? a : b)

static struct class *hello_class;

//3 实现对应的 drv_open/drv_read/drv_write 等函数，填入 file_operations 结构体
static int hello_open(struct inode *node, struct file *file)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    return 0;
}
    
static ssize_t hello_read(struct file *file, char __user *buf, size_t size, loff_t *offest)
{
    int err = 0;
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    err = copy_to_user(buf,kernel_buf,MIN(1024,size));
    return MIN(1024,size);
}

static ssize_t hello_write(struct file *file, const char __user *buf, size_t size, loff_t *offest)
{
    int err = 0;
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    err = copy_from_user(kernel_buf,buf,MIN(1024,size));
    return MIN(1024,size);
}
    
static int hello_release(struct inode *node, struct file *file)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    return 0;
}

//2 定义自己的 file_operations 结构体
static struct file_operations hellow_drv_ops = {
    .owner		= THIS_MODULE,
    .open       = hello_open,
    .read       = hello_read,
    .write      = hello_write,
    .release    = hello_release, 
};

// 4 把 file_operations 结构体告诉内核:register_chrdev
//5 谁来注册驱动程序啊?得有一个入口函数:安装驱动程序时，就会去调用这个入口函数 
static int __init hello_init(void)
{
    int err = 0;
    int rc;
    dev_t devid;
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    //major = register_chrdev(0,"hello_drv",&hellow_drv_ops); /* /dev/hello_drv */

    rc = alloc_chrdev_region(&devid,0,1,"hello_drv");
    major = MAJOR(devid);
    cdev_init(&cdev_hello,&hellow_drv_ops);
    cdev_add(&cdev_hello,devid,1);

    hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
        printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
        //unregister_chrdev(major,"hello_drv");
        cdev_del(&cdev_hello);
        unregister_chrdev_region(MKDEV(major,0),1);
        return -1;
    }
	device_create(hello_class,NULL,MKDEV(major,0),NULL,"hello_drv");   //创建设备节点/dev/hello_drv
    

    return 0;

}

// 6 有入口函数就应该有出口函数:卸载驱动程序时，出口函数调用 unregister_chrdev
static  void __exit hello_exit(void)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    device_destroy(hello_class,MKDEV(major,0));
    class_destroy(hello_class);
    //unregister_chrdev(major,"hello_drv");
    cdev_del(&cdev_hello);
    unregister_chrdev_region(MKDEV(major,0),1);
}

//7 其他完善:提供设备信息，自动创建设备节点:class_create, device_create
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("silence");