
#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_kfifo-objs := my_demo_kfifo.o
obj-m := mydemo_kfifo.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;