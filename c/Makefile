CC = gcc
CFLAGS = -Wall -Werror -fpic -fsigned-char

all : logipilib.o logibonelib.o

clean:
	rm -f *.a *.o

install_logipi : liblogipi.so
	mv liblogipi.so /usr/lib/liblogi.so
	cp logilib.h /usr/include

install_logibone : liblogibone.so
	mv liblogibone.so /usr/lib/liblogi.so
	cp logilib.h /usr/include

logipilib.o: logipilib.c
	$(CC) -c $(CFLAGS) logipilib.c

logibonelib.o: logibonelib.c
	$(CC) -c $(CFLAGS) logibonelib.c

liblogibone.so : logibonelib.o
	$(CC) -shared -o liblogibone.so logibonelib.o

liblogipi.so : logipilib.o
	$(CC) -shared -o liblogipi.so logipilib.o

