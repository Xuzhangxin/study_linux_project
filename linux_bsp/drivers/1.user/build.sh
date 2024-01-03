#!/bin/sh

# make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
ARG1=$1
CURRENT_DIR=$(cd `dirname $0`; pwd)

echo "CURRENT_DIR is $CURRENT_DIR"
echo "ARG1 is $ARG1"

if [ "$ARG1" = "" ] || [ "$ARG1" = "build" ]
then
    echo "================================build driver start================================"
    cd $CURRENT_DIR/driver
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    echo "================================build driver done================================"
    echo "================================build app start================================"
    cd $CURRENT_DIR/app
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    echo "================================build app done================================"
elif [ "$ARG1" = "clean" ]
then
    echo "================================clean driver start================================"
    cd $CURRENT_DIR/driver;
    make clean
    echo "================================clean driver done================================"
    echo "================================clean app start================================"
    cd $CURRENT_DIR/app;
    make clean
    echo "================================clean app done================================"
fi



