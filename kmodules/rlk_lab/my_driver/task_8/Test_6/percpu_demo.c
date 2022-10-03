#include <linux/module.h>
#include <linux/init.h>
#include <linux/percpu.h>
#include <linux/cpumask.h>

static DEFINE_PER_CPU(long,cpuvar) = 10;
static long __percpu *cpualloc;

static int __init my_init(void)
{
    int cpu;
    pr_info("module loaded at 0x%p\n",my_init);

    for_each_possible_cpu(cpu) {
        pr_info("init_1:cpuvar on cpu%d = %ld\n",cpu,get_cpu_var(cpuvar));
        per_cpu(cpuvar,cpu) = 15;
        pr_info("init_2:cpuvar on cpu%d = %ld\n",cpu,get_cpu_var(cpuvar));
        put_cpu_var(cpuvar);
    }
    pr_info("init_3:cpuvar on cpu%d = %ld\n",cpu,get_cpu_var(cpuvar));
    __this_cpu_write(cpuvar,20);
    pr_info("init_4:cpuvar on cpu%d = %ld\n",cpu,get_cpu_var(cpuvar));
    
    cpualloc = alloc_percpu(long);

    for_each_possible_cpu(cpu) {
        *per_cpu_ptr(cpualloc,cpu) = 100;
        pr_info("init:cpu:%d cpualloc = %ld\n",cpu,*per_cpu_ptr(cpualloc,cpu));
    }
    return 0;
}

static void __exit my_exit(void)
{
    int cpu;
    pr_info("exit module\n");
    for_each_possible_cpu(cpu) {

        pr_info("cpuvar cpu%d = %ld\n",cpu,per_cpu(cpuvar,cpu));
        pr_info("exit:cpualloc%d = %ld\n",cpu,*per_cpu_ptr(cpualloc,cpu));
    }

    free_percpu(cpualloc);
    pr_info("Bye:module unloaded from 0x%p\n",my_exit);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("lei6.yao");
MODULE_LICENSE("GPL v2");