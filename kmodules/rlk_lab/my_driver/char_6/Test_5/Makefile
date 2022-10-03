
#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_noblock_kfifo-objs := my_demo_noblock_kfiflo.o
obj-m := mydemo_noblock_kfifo.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;