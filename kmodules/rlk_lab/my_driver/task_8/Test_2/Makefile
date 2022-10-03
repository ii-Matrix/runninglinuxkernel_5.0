
BASEINCLUDE ?= /lib/modules/`uname -r`/build

kthread_demo-objs := my_kthread_demo.o

obj-m	:=   kthread_demo.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


