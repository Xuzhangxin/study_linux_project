PWD_DIR:=$(shell pwd)
KERNELDIR := $(PWD_DIR)/../../../linux-imx-4.1.15-2.1.0-g3dc0a4b-v2.7
CURRENT_PATH := $(shell pwd)
	obj-m := user_chrdev.o 

build: kernel_modules 
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
