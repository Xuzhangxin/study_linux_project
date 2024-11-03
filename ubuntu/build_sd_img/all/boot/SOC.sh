#!/bin/sh
format=1.0

board=fire-imx6ull-npi

bootloader_location=dd_uboot_boot
bootrom_gpt=

dd_spl_uboot_count=
dd_spl_uboot_seek=
dd_spl_uboot_conf=notrunc
dd_spl_uboot_bs=
dd_spl_uboot_backup=

dd_uboot_count=
dd_uboot_seek=2
dd_uboot_conf=notrunc
dd_uboot_bs=512
dd_uboot_emmc_backup=/opt/backup/uboot/u-boot-mmc.imx
dd_uboot_nand_backup=/opt/backup/uboot/u-boot-nand.imx

boot_fstype=fat
conf_boot_startmb=4
conf_boot_endmb=40
sfdisk_fstype=0xE

conf_root_device=/dev/mmcblk0

boot_label=BOOT
rootfs_label=rootfs

#Kernel
dtb=
serial_tty=
usbnet_mem=

#Advanced options
#disable_ssh_regeneration=true




