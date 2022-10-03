#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include  <linux/kernel.h>
#include <asm/page.h>

static int mem = 1024;
#define MB (1024*1024)

static int __init my_init(void)
{
    char *kbuf;
    struct page *page;
    unsigned long order;
    unsigned long size;
    char *vm_buff;

    for(size = PAGE_SIZE,order = 0 ;order < MAX_ORDER;order++,size *= 2) {
        pr_info("order = %2lu,page = %5lu,size = %8lu",order,size/PAGE_SIZE,size);
        kbuf = (char *)__get_free_pages(GFP_ATOMIC,order);
        page = alloc_pages(GFP_ATOMIC,order);

        if(!kbuf || !page)
        {
            pr_err("...__get_free_pages or alloc_pages failed\n");
            break;
        }
        pr_info("..__get_free_pages and alloc_pages OK\n");
        pr_info("get_free_pages:phys_addr = 0x%llx ,vir_addr = 0x%llx \n",__pa(&kbuf),&kbuf);
        pr_info("alloc_pages:phys_addr = 0x%llx,vir_addr = 0x%llx\n",page_to_phys(page),page_to_virt(page));
        
        memset((char *)page_to_virt(page),0x54,size);
        pr_info("this  = %s\n",((char *)page_to_virt(page)));

        free_pages((unsigned long)kbuf,order);
        __free_pages(page,order);
    }

    for(size = PAGE_SIZE,order = 0;order < MAX_ORDER;order++,size *= 2) {
        pr_info(" order = %2ld,pages=%5lu,size = %8lu ",order,size/PAGE_SIZE,size);
        kbuf = kmalloc((size_t)size,GFP_ATOMIC);
        if(!kbuf) {
            pr_err(".. kmalloc failed\n");
            break;
        }
        pr_info("... kmalloc OK\n");
        kfree(kbuf);
    }
/*
    for(size = 20 * MB;size <= mem * MB;size += 20 * MB) {
        pr_info(" page=%6lu,size = %8lu",size/PAGE_SIZE,size);

        vm_buff = vmalloc(size);
        if(!vm_buff) {
            pr_err("...vmalloc failed\n");
            break;
        }
        pr_info("... vmalloc OK\n");
        vfree(vm_buff);
    }*/

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("Module exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("lei6 yao");
MODULE_LICENSE("GPL v2");