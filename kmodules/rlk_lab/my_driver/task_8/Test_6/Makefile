
BASEINCLUDE ?= /lib/modules/`uname -r`/build

demopercpu-objs := percpu_demo.o

obj-m	:=   demopercpu.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


