#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *task[NR_CPUS];

static void show_reg(void)
{
    unsigned int spsr,sp,el;

    asm("mrs %0,spsr_el1" : "=r" (spsr) :: "cc");
    asm("mov %0,sp" : "=r" (sp) :: "cc");
    asm("mrs %0, CurrentEL" : "=r" (el) :: "cc");

    printk("spsr:0x%x,sp:0x%x,el=%d\n",spsr,sp,el >> 2);
}

static void show_prio(void)
{
    struct task_struct *task = current;

    printk("%s pid:%d,nice:%d,prio:%d,static_prio:%d,normal_prio:%d\n",
            task->comm,task->pid,
            PRIO_TO_NICE(task->static_prio),
            task->prio,task->static_prio,
            task->normal_prio);
}

static void print_cpu(char *s)
{
    preempt_disable();
    pr_info("%s cpu = %d.\n",s,smp_processor_id());
    preempt_enable();
}

static int thread_func(void *t)
{
    do {
        print_cpu("SLEEP in Thread Function ");
        msleep_interruptible(2000);
        print_cpu("msleep over in Thread Function");
        print_cpu("running");
        show_reg();
        show_prio();
    }while(!kthread_should_stop());

    return 0;
}

static int __init my_init(void)
{
    int i;
    for_each_online_cpu(i) {
        task[i] = kthread_create(thread_func,NULL,"kdemo/%d",i);
        if(!task[i]) {
            pr_info("Failed to generate a kernel thread\n");
            return -1;
        }
        kthread_bind(task[i],i);
        pr_info("About to wake up and run the thread for cpu = %d\n",i);
        wake_up_process(task[i]);
        pr_info("Starting thread for cpu %d",i);
        print_cpu("on");
    }
    return 0;
}

static void __exit my_exit(void)
{
    int i;
    for_each_online_cpu(i) {
        pr_info("Kill Thread %d",i);
        kthread_stop(task[i]);
        print_cpu("Kill was done on");
    }
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("lei6.yao");
MODULE_LICENSE("GPL v2");