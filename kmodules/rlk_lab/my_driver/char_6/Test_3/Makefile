
#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_fifo-objs := my_demo_fifo.o
obj-m := mydemo_fifo.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;