CC = gcc


all: wb_utils

clean:
	rm -f *.a *.o wb_utils
	
wb_utils :  wb_utils.c
	$(CC) -fsigned-char -o $@ wb_utils.c -llogi
