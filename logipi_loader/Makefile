#Compiler to use
CC=gcc
#Compiler Flags
CFLAGS=-c #-Wall
#Linker Flags
LDFLAGS= #-lrt

#version flag
PI_VER=

all: logi_loader

logi_loader: serial_fpga_loader.o
	$(CC) -o  logi_loader serial_fpga_loader.o 

serial_fpga_loader.o: serial_fpga_loader.c
	$(CC) $(CFLAGS) -D$(PI_VER) serial_fpga_loader.c

install: logi_loader
	install logi_loader /usr/bin
clean:
	rm -rf *o logi_loader
