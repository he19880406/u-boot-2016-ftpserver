#!/bin/sh

export CFLAGS="-O2 -pipe -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -w"

make clean
make rpi_3_32b_defconfig
make
