Logi-tools
==========

Software tools (loader, libraries) to be installed on the host platform (logipi/logibone).


LOGI-BONE Instructions

To install the logi-bone tools, run install_logibone as sudo. This will flash the cape eeprom, install the loader, compile the communication library and setup the python library


LOGI-PI Instructions

To install the logi-pi tools, run install_logipi as sudo. This will install the loader, compile the communication library and install the python library.

For PI2 owners using the LOGIPI R1.1, you'll need to re-compile the loader using the following procedure (in logi-tools):

cd unified_loader 

make clean

make logipi_loader PI_VER=PI2

sudo make install

To enable SPI on latest raspbian distro, edit /boot/config.txt (as sudo) and add :

dtparam=spi=on

To enable I2C on latest raspbian distro, edit /boot/config.txt (as sudo )and add :

dtparam=i2c_arm=on

to the file.

also edit /etc/modules (as sudo) and add the line :

i2c-dev
