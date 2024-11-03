#bin/bash
sudo apt-get install dosfstools dump parted kpartx
mkdir ./lhf
mkdir ./lhf/boot 
mkdir ./lhf/root
echo "正在创建文件,需要一定时间,请勿中断"
dd if=/dev/zero of=./imx6ull.img bs=512 count=15728640
sudo parted imx6ull.img --script -- mklabel msdos
sudo parted imx6ull.img --script -- mkpart primary fat32 10240s 92159s
sudo parted imx6ull.img --script -- mkpart primary ext4 92164s -1
sudo losetup -f --show imx6ull.img
sudo kpartx -va /dev/loop0
sudo mkfs.vfat /dev/mapper/loop0p1
sudo mkfs.ext4 /dev/mapper/loop0p2
sudo mount /dev/mapper/loop0p1 ./lhf/boot/
sudo mount /dev/mapper/loop0p2 ./lhf/root/
sudo dd if=./all/u-boot-mmc.imx of=/dev/loop0 bs=512 seek=2 conv=fsync
sudo tar -vxf ./all/boot.tar -C ./lhf/boot/
sudo tar -vxf ./all/rootfs.tar -C ./lhf/root/
sudo umount ./lhf/boot ./lhf/root
sudo kpartx -d /dev/loop0
sudo losetup -d /dev/loop0
echo "正在打包,需要一定时间,请勿中断"
xz -zf ./imx6ull.img
rm -rf mkdir ./lhf/boot ./lhf/root ./lhf
