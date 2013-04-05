CC = gcc


all: fifo_test

clean:
	rm -f *.a *.o fifo_test

fifo_test : fifo_test.c fifolib.c
	$(CC) -o $@ fifo_test.c fifolib.c
