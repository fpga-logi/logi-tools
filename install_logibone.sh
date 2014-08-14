#!/bin/bash

SCRIPT_DIR=`pwd`

sudo apt-get update

echo "Setup cape eeprom ID"
cd init_logibone
sudo ./init_eeprom.sh
cd ${SCRIPT_DIR}

echo "Install dependencies"
sudo apt-get install gcc make git

echo "Install logi_loader"
cd logibone_loader/user_space
make
sudo make install
cd ${SCRIPT_DIR}

echo "Install python support"
sudo apt-get install python-dev
cd python/logibone
python2 setup.py build
sudo python2 setup.py install
cd ${SCRIPT_DIR}

echo "Install logi C library"
cd c
sudo make install_logibone
cd ${SCRIPT_DIR}

