#!/bin/bash
echo "rootfs mount begin" >> /dev/kmsg
mount -t proc /proc
    /mnt/nfs/ubuntu/ubuntu_rootfs/proc
mount -t sysfs /sys
    /mnt/nfs/ubuntu/ubuntu_rootfs/sys
mount -o bind /dev /mnt/nfs/ubuntu/ubuntu_rootfs/dev
mount -o bind /dev/pts
    /mnt/nfs/ubuntu/ubuntu_rootfs/dev/pts
chroot /mnt/nfs/ubuntu/ubuntu_rootfs
echo "rootfs mount begin" >> /dev/kmsg
