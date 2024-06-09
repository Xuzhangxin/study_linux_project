CURRENT_PATH := $(shell pwd)
	obj-m := pinctrl_led.o 
KERNELDIR := $(CURRENT_PATH)/../../../linux-imx-4.1.15-2.1.0-g3dc0a4b-v2.7
#/home/xzx/share/project_ipc/study_linux_project/linux_bsp/linux-imx-4.1.15-2.1.0-g3dc0a4b-v2.7/arch/arm/boot/dts/imx6ull-alientek-emmc.dts
build: kernel_modules 
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
