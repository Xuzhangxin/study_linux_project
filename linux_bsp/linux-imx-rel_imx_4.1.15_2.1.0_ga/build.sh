#!/bin/sh
make clean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx_v7_mfg_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16


