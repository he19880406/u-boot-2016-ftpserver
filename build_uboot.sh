#!/bin/sh

export CROSS_COMPILE=/opt/FriendlyARM/toolschain/4.4.3/bin/arm-linux-
export ARCH=arm

make clean
make mini2440_defconfig
make 

