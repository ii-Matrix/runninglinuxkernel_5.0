
#BASEINCLUDE ?= /lib/modules/'uname -r'/build
BASEINCLUDE ?= /lib/modules/5.0.0+/build
KERN_DIR ?= /lib/modules/'uname -r'/build
#/home/lighthouse/01_runlinux_5.0/runninglinuxkernel_5.0


hello-objs := hellow_drv.o
obj-m := hello.o

all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
