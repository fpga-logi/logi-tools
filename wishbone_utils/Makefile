CC = gcc


all: wb_utils

clean:
	rm -f *.a *.o wb_utils
	
wb_utils :  wb_utils.c
	$(CC) -o $@ wb_utils.c -llogi
