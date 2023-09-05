# To build the kernel module outside kernel tree, run `make`
# In the kernel source tree, the Makefile these and include this 
# Makefile once again.
# This conditional select whether we are being included from the kernel 
# Makefile or not.
#

ifeq ($(KERNELRELEASE),)
	#Assume the source tree is where the running kernel was built
	#Set the kerneldir in the environment if its elsewhere
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	#The current directory is passed to sub-makes as argument
	PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c *.mod *.order *.symvers .tmp_versions

.PHONY: modules modules_install clean

else
	#called from the kernel build system just declare what our module are
	obj-m :=led_drv.o led_drv_1.o led_drv_2.o led_drv_3.o led_drv_4.o
endif
