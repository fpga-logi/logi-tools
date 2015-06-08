#Compiler to use
CC=gcc
#Linker Flags
LDFLAGS= #-lrt
PI_VER=PI1


logipi_loader: logi_loader.c i2c_loader.c bit_bang_loader.c
	$(CC) $(LDFLAGS) -DLOGIPI -D$(PI_VER) -o  logi_loader logi_loader.c i2c_loader.c bit_bang_loader.c

logibone_loader: logi_loader.c i2c_loader.c bit_bang_loader.c
	$(CC) $(LDFLAGS) -DLOGIBONE -o logi_loader logi_loader.c i2c_loader.c bit_bang_loader.c

install: logi_loader
	install logi_loader /usr/bin
clean:
	rm -rf *o logi_loader
