#!/bin/sh

export CROSS_COMPILE=arm-linux-
export ARCH=arm

make clean
make mini2440_defconfig
make 

