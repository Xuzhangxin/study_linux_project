CURRENT_PATH := $(shell pwd)
	obj-m := led.o 
KERNELDIR := $(CURRENT_PATH)/../../../linux-imx-4.1.15-2.1.0-g3dc0a4b-v2.7

build: kernel_modules 
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
