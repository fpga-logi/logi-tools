CC = gcc


all: wb_utils

clean:
	rm -f *.a *.o wb_utils

wishbone_wrapper.o: wishbone_wrapper.c
	$(CC) -c wishbone_wrapper.c
	
wb_utils :  wb_utils.c wishbone_wrapper.o
	$(CC) -o $@ wb_utils.c wishbone_wrapper.o
