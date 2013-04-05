#Compiler to use
CC=gcc
#Compiler Flags
CFLAGS=-c #-Wall

all: setup_gpmc

setup_gpmc: setup_gpmc.o
	$(CC) -o setup_gpmc setup_gpmc.o

setup_gpmc.o: setup_gpmc.c
	$(CC) $(CFLAGS) setup_gpmc.c


clean:
	rm -rf *o setup_gpmc
