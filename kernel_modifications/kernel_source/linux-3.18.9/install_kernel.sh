# Charles Sedgwick
# This script automates the installation of a custom kernel under arch linux

#! /bin/bash

KERNEL_VER=3.18.9
KERNEL_NAME=CMPT496
SOURCE_DIR=.

make modules_install
cp -v $SOURCE_DIR/arch/x86/boot/bzImage /boot/vmlinuz-$KERNEL_NAME
mkinitcpio -k $KERNEL_VER-$KERNEL_NAME -c /etc/mkinitcpio.conf -g/boot/initramfs-$KERNEL_NAME.img
cp -v System.map /boot/System.map-$KERNEL_NAME.img
ln -vsf /boot/System.map-$KERNEL_NAME /boot/System.map
