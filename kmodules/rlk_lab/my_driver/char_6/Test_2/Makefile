
#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_misc-objs := my_demo_misc.o
obj-m := mydemo_misc.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;