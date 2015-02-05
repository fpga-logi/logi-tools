#Compiler to use
CC=gcc
#Linker Flags
LDFLAGS= #-lrt

logipi_loader: logi_loader.c
	$(CC) $(LDFLAGS) -DLOGIPI -o  logi_loader logi_loader.c 

logibone_loader: logi_loader.c
	$(CC) $(LDFLAGS) -DLOGIBONE -o logi_loader logi_loader.c

install: logi_loader
	install logi_loader /usr/bin
clean:
	rm -rf *o logi_loader
