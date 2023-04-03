#!/bin/bash

case $1 in
insert)
make
sudo insmod hello.ko
sudo chmod 777 /dev/test_file
;;
remove)
sudo rmmod hello
make clean
;;
test)
sudo rmmod hello
make clean
make
sudo insmod hello.ko
sudo chmod 777 /dev/test_file
cat /dev/test_file
dmesg
;;
esac

