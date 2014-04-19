CC = gcc
CFLAGS = -Wall -Werror -fpic

all : logipilib.o logibonelib.o

clean:
	rm -f *.a *.o

logipilib.o: logipilib.c
	$(CC) -c $(CFLAGS) logipilib.c

logibonelib.o: logibonelib.c
	$(CC) -c $(CFLAGS) logibonelib.c

liblogibone.so : logibonelib.o
	$(CC) -shared -o liblogibone.so logibonelib.o

liblogipi.so : logipilib.o
	$(CC) -shared -o liblogipi.so logipilib.o
