#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/pid.h>

static int pid;
module_param(pid,int,S_IRUGO);


static void printit(struct task_struct *tsk)
{
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    int j = 0;
    unsigned long start,end,length;
    mm = tsk->mm;
    pr_info("mm = %p\n",mm);
    vma = mm->mmap;

    down_read(&mm->mmap_sem);
    pr_info("vmas:vma    start    end      length\n");

    while(vma) {
        j++;
        start = vma->vm_start;
        end = vma->vm_end;
        length = end - start;
        pr_info("%6d:%16p  %12lx  %12lx  %8lx = %8ld\n",j,vma,start,end,length,length);
        vma = vma->vm_next;
    }
    up_read(&mm->mmap_sem);
}

static int __init my_init(void)
{
    struct task_struct *task;
    if(pid == 0) {
        task = current;
        pid = current->pid;
    } else {
        //task = pid_task(find_vpid(pid),PIDTYPE_PID);
        task = pid_task(find_get_pid(pid),PIDTYPE_PID);
    }
    if(!task)
        return -1;
    pr_info("Examining vma's for pid = %d,command = %s\n",pid,task->comm);
    printit(task);
    return 0;
}


static void __exit my_exit(void)
{
    pr_info("Module Unloading\n");
}
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("silence");