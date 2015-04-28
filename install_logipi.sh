#!/bin/bash

SCRIPT_DIR=`pwd`

sudo apt-get update

echo "Enable SPI"
sed -i s,blacklist spi-bcm2708,#blacklist spi-bcm2708,g /etc/modprobe.d/raspi-blacklist.conf
modprobe spi-bcm2708
modprobe spidev

echo "Install dependencies"
sudo apt-get install gcc make git

echo "Install logi_loader"
cd unified_loader
make logipi_loader
sudo make install
cd ${SCRIPT_DIR}

echo "Install python support"
sudo apt-get install python-dev
cd python/logipi
python2 setup.py build
sudo python2 setup.py install
cd ${SCRIPT_DIR}

echo "Install logi C library"
cd c
sudo make install_logipi

