#!/bin/sh
dtc -O dtb -o BB-BONE-LOGIBONE-00A1.dtbo -b 0 -@ BB-BONE-LOGIBONE-00A1.dts
sudo cp BB-BONE-LOGIBONE-00A1.dtbo /lib/firmware
sudo sh -c "echo BB-BONE-LOGIBONE:00A1 > /sys/devices/bone_capemgr.9/slots "
