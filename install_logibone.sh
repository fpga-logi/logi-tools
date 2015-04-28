#!/bin/bash

SCRIPT_DIR=`pwd`

sudo apt-get update

echo "Settin up cape eeprom ID ..............."
cd init_logibone
sudo ./init_eeprom.sh
cd ${SCRIPT_DIR}

echo "Installing dependencies..............."
sudo apt-get install gcc make git

echo "Installing logi_loader..............."
cd unified_loader
make logibone_loader
sudo make install
cd ${SCRIPT_DIR}

echo "Installing python support..............."
sudo apt-get install python-dev
cd python/logibone
python2 setup.py build
sudo python2 setup.py install
cd ${SCRIPT_DIR}

echo "Installing logi C library..............."
cd c
sudo make install_logibone
cd ${SCRIPT_DIR}

