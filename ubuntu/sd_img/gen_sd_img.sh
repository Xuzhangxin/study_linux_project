#bin/bash
sudo apt-get install dosfstools dump parted kpartx
mkdir ./boot
mkdir ./rootfs
echo "begin create imx6ull_all.img"
# 1gB
dd if=/dev/zero of=imx6ull_all.img bs=1K count=1048576
# 非交互式创建MS-DOS 类型的分区表
sudo parted imx6ull_all.img --script -- mklabel msdos
# make fat32 filesystem to kernel and dtb
# 16384 block = 0x00800000 Bytes / 512   这感觉还要考虑boot，这样相当于默认boot8MB
# 114688 block = 0x03800000 Bytes / 512
sudo parted imx6ull_all.img --script -- mkpart primary fat32 8M 42M

# make ext4 system to rootfs
sudo parted imx6ull_all.img --script -- mkpart primary ext4 42M -1 

# 先删除所有映射，镜像文件虚拟为块设备
sudo losetup -D
sudo losetup -f --show imx6ull_all.img
#创建loop分区映射表
sudo kpartx -va /dev/loop0
sync
sleep 2
#建立文件系统 存放 kernel dtb
sudo mkfs.vfat /dev/mapper/loop0p1     
#建立文件系统 存放 rootfs   
sudo mkfs.ext4 /dev/mapper/loop0p2        
sync
sleep 2

# 挂载镜像，并拷贝固件到挂载目录，更改img镜像内容
sudo mount /dev/mapper/loop0p1 ./boot/
sudo mount /dev/mapper/loop0p2 ./rootfs/
# 拷贝boot 跳过1024字节
sudo dd if=./all/u-boot-imx6ull-14x14-ddr512-emmc.imx of=/dev/loop0 bs=1K seek=1 conv=fsync 
# 拷贝kernel和dtb
sudo cp ./all/zImage ./boot/
sudo cp ./all/imx6ull-alientek-emmc.dtb ./boot/
# 拷贝rootfs
# sudo cp ./all/ubuntu_rootfs.tar.gz ./rootfs/
sudo tar -vxf ./all/ubuntu-base-16.04.6-base-armhf.tar.gz -C ./rootfs
# sudo tar --strip-components=3 -C ./rootfs -vxf ./all/ubuntu_rootfs.tar.gz binary/boot/filesystem.dir
sync
#卸载挂载分区，删除分区映射 解除挂载，打包镜像，删除挂载文件夹
# sudo umount ./boot ./rootfs
# sudo kpartx -d /dev/loop0
# sudo losetup -d /dev/loop0
echo "正在打包,需要一定时间,请勿中断"
## bzip2 ./imx6ull_all.img
# sudo rm -rf ./boot ./rootfs
