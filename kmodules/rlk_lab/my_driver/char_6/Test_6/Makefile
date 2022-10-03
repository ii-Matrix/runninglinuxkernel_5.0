#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_waitqueue-objs := my_demo_waitqueue.o
obj-m := mydemo_waitqueue.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;