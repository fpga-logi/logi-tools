Logi-tools
==========

Software tools (loader, libraries) to be installed on the host platform (logipi/logibone).


LOGI-BONE Instructions

To install the logi-bone tools, run install_logibone as sudo. This will flash the cape eeprom, install the loader, compile the communication library and setup the python library


LOGI-PI Instructions

To install the logi-pi tools, run install_logipi as sudo. This will flash the cape eeprom, install the loader, compile the communication library and install the python library.

For PI2 owners : For PI2 only the loader needs to be updated and device-tree setup so spi is enabled (disabled by default). To update the loader after the install_logipi script was run, go to logipi_loader and run the following commands:

make clean
make PI_VER=PI2
sudo make install

To enable SPI on latest raspbian distro, edit /boot/config.txt (as sudo) and add :

dtparam=spi=on

To enable I2C on latest raspbian distro, edit /boot/config.txt (as sudo )and add :

dtparam=i2c_arm=on

to the file.

also edit /etc/modules (as sudo) and add the line :

i2c-dev
