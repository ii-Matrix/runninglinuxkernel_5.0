#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/poll.h>

#define DEMO_NAME "my_demo_io"

#define MAX_DEVICE_BUFFER_SIZE 64
#define MYDEMO_MAX_DEVICES 8


static dev_t dev;
static struct cdev *demo_cdev;

struct mydemo_device {
    char name[64];
    struct device *dev;
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
    struct kfifo mydemo_kfifo;
};
static struct mydemo_device *mydemo_device[MYDEMO_MAX_DEVICES];

struct mydemo_private_device {
    struct mydemo_device *device;
    char name[64];
};

static int demodrv_open(struct inode *inode, struct file *file)
{
    unsigned int minor = iminor(inode);
    struct mydemo_private_device *data;
    struct mydemo_device *device = mydemo_device[minor];
    int ret;
    printk("%s ,major = %d,minor = %d,device = %s\n",__func__,MAJOR(inode->i_rdev),MINOR(inode->i_rdev),device->name);

    data = kmalloc(sizeof(struct mydemo_private_device),GFP_KERNEL);
    if(!data)
        return -ENOMEM;

    sprintf(data->name,"private_data_%d",minor);
    data->device = device;
    file->private_data = data;

    return 0;
}

//int (*release) (struct inode *, struct file *);
static int demodrv_release(struct inode *inode, struct file *file)
{
    struct mydemo_private_device *data = file->private_data;
    kfree(data);
    return 0;
}

//	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);

static ssize_t
demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
    struct mydemo_private_device *data = file->private_data;
    struct mydemo_device *device = data->device;

    int actual_readed;
    int ret;

    printk("%s enter,ppos = %s\n",__func__,*ppos);

    if(kfifo_is_empty(&device->mydemo_kfifo)) {
        if(file->f_flags & O_NONBLOCK)
            return -EAGAIN;
        
        printk("%s,pid = %d,going to sleep\n",__func__,current->pid);
        ret = wait_event_interruptible(device->read_queue,!kfifo_is_empty(&device->mydemo_kfifo));
        if(ret)
            return EIO;
    }


   // ret = copy_to_user(buf,device_buffer + *ppos,need_read);
   ret = kfifo_to_user(&device->mydemo_kfifo,buf,lbuf,&actual_readed);

    if(ret)
        return -EIO;
    
    if(!kfifo_is_full(&device->mydemo_kfifo))
        wake_up_interruptible(&device->write_queue);

    printk("%s,pid = %s,actual_readed = %d,pos = %lld\n",__func__,current->pid,actual_readed,*ppos);

    return actual_readed;
}
//	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops)
{
    struct mydemo_private_device *data = file->private_data;
    struct mydemo_device *device = data->device;

    unsigned int actual_write;
    int ret;

    printk("%s enter fops = %s\n",__func__,*f_ops);
    if(kfifo_is_full(&device->mydemo_kfifo)) {
        if(file->f_flags & O_NONBLOCK)
            return -EAGAIN;

        printk("%s,pid = %d,going to sleep\n",__func__,current->pid);
        ret = wait_event_interruptible(device->write_queue,!kfifo_is_full(&device->mydemo_kfifo));
        if(ret)
            return ret;
       }

    // ret = copy_from_user(device_buffer + *f_ops, buf, need_write);
    ret = kfifo_from_user(&device->mydemo_kfifo,buf,count,&actual_write);
    if(ret)
        return -EIO;

    if(!kfifo_is_empty(&device->mydemo_kfifo))
        wake_up_interruptible(&device->read_queue);

    printk("%s:pid = %d,actual_write = %d,ppos=%lld,ret = %d\n",__func__,current->pid,actual_write,*f_ops,ret);
    return actual_write;
}
static unsigned int demodrv_poll(struct file *file,poll_table *wait)
{
    int mask = 0;
    struct mydemo_private_device *data = file->private_data;
    struct mydemo_device *device = data->device;

    poll_wait(file,&device->read_queue,wait);
    poll_wait(file,&device->write_queue,wait);

    if(!kfifo_is_empty(&device->mydemo_kfifo))
        mask |= POLLIN | POLLRDNORM;
    if(!kfifo_is_full(&device->mydemo_kfifo))
        mask |= POLLOUT | POLLWRNORM;

    return mask;
}

static const struct file_operations demodrv_fops = {
    .owner = THIS_MODULE,
    .open = demodrv_open,
    .release = demodrv_release,
    .read = demodrv_read,
    .write = demodrv_write,
    .poll = demodrv_poll,
};

static int __init simple_char_init(void)
{
    int ret;
    int i;

    struct mydemo_device *device;
    ret = alloc_chrdev_region(&dev,0,MYDEMO_MAX_DEVICES,DEMO_NAME);
    if(ret) {
        printk("failed to allocate char device region\n");
        return ret;
    }

    demo_cdev = cdev_alloc();
    if(!demo_cdev) {
        printk("cdev_alloc failed\n");
        goto unregister_chrdev;
    }

    cdev_init(demo_cdev,&demodrv_fops);

    ret = cdev_add(demo_cdev,dev,MYDEMO_MAX_DEVICES);
    if(ret) {
        printk("cdev_add failed\n");
        goto chrdev_fail;
    }

    for(i = 0;i < MYDEMO_MAX_DEVICES;i++) {
            device = kmalloc(sizeof(struct mydemo_device),GFP_KERNEL);
            if(!device) {
                ret = -ENOMEM;
                goto free_device;
            }

        sprintf(device->name,"%s%d",DEMO_NAME,i);
        mydemo_device[i] = device;
        init_waitqueue_head(&device->read_queue);
        init_waitqueue_head(&device->write_queue);

        ret = kfifo_alloc(&device->mydemo_kfifo,MAX_DEVICE_BUFFER_SIZE,GFP_KERNEL);
        if(ret) {
            ret = -ENOMEM;
            goto free_kfifo;
        }
        printk("mydemo_fifo = %p\n",&device->mydemo_kfifo);

    }
    printk("succeeded register char device: %s\n",DEMO_NAME);
    return 0;

free_kfifo:
    for(i = 0; i< MYDEMO_MAX_DEVICES;i++)
        if(&mydemo_device[i]->mydemo_kfifo)
            kfifo_free(&mydemo_device[i]->mydemo_kfifo);

free_device:
    for(i = 0;i < MYDEMO_MAX_DEVICES;i++)
        if(mydemo_device[i])
            kfree(mydemo_device[i]);

chrdev_fail:
    cdev_del(demo_cdev);

unregister_chrdev:
    unregister_chrdev_region(dev,MYDEMO_MAX_DEVICES);
    return ret;
}

static void __exit simple_char_exit(void)
{
    int i;
    printk("removing device\n");

    if(demo_cdev)
        cdev_del(demo_cdev);
    
    unregister_chrdev_region(dev,MYDEMO_MAX_DEVICES);

    for(i = 0;i < MYDEMO_MAX_DEVICES;i++) {
        if(mydemo_device[i])
            kfree(mydemo_device[i]);
    }
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("silence");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simple character device");