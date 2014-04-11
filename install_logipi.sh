#!/bin/bash

echo "Enable SPI"
sed -i s,blacklist spi-bcm2708,#blacklist spi-bcm2708,g /etc/modprobe.d/raspi-blacklist.conf
modprobe spi-bcm2708
modprobe spidev

echo "Install dependencies"
sudo apt-get install gcc make git

echo "Install logi_loader"
git clone https://github.com/fpga-logi-dev/logi-tools.git
cd logi-tools/logipi_loader
make
sudo make install

echo "Install python support"
sudo apt-get install python-dev
cd logi-tools/python/logipi
python2 setup.py build
sudo python2 setup.py install

echo "Running Blink Led demo"
logi_loader blink_led.bit
