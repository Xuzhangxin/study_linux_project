cmd_libbb/makedev.o := arm-linux-gnueabihf-gcc -Wp,-MD,libbb/.makedev.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=KBUILD_STR(1.31.0)"  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin-printf -Os     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(makedev)"  -D"KBUILD_MODNAME=KBUILD_STR(makedev)" -c -o libbb/makedev.o libbb/makedev.c

deps_libbb/makedev.o := \
  libbb/makedev.c \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/stdc-predef.h \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config/big/endian.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/nommu.h) \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/lib/gcc/arm-linux-gnueabihf/4.9.4/include-fixed/limits.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/lib/gcc/arm-linux-gnueabihf/4.9.4/include-fixed/syslimits.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/limits.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/features.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/sys/cdefs.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/wordsize.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/gnu/stubs.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/gnu/stubs-hard.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/posix1_lim.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/local_lim.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/linux/limits.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/posix2_lim.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/xopen_lim.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/stdio_lim.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/byteswap.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/byteswap.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/types.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/typesizes.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/byteswap-16.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/endian.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/endian.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/lib/gcc/arm-linux-gnueabihf/4.9.4/include/stdint.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/stdint.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/wchar.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/lib/gcc/arm-linux-gnueabihf/4.9.4/include/stdbool.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/unistd.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/posix_opt.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/environments.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/lib/gcc/arm-linux-gnueabihf/4.9.4/include/stddef.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/bits/confname.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/getopt.h \
  /home/xzx/share/project_ipc/toolchain/gcc-linaro-4.9.4/arm-linux-gnueabihf/libc/usr/include/sys/sysmacros.h \

libbb/makedev.o: $(deps_libbb/makedev.o)

$(deps_libbb/makedev.o):
