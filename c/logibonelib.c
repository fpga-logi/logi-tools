#include "logilib.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

int fd ;

int logi_open(){
	fd = open("/dev/logibone_mem", O_RDWR | O_SYNC);
	if(fd < 0){
		printf("Failed to open /dev/logibone_mem");
		return -1 ;
	}

	return 1 ;
}

void logi_close(){
	close(fd);
}

int logi_write(unsigned char * buffer, unsigned int length, unsigned int address){
	int count = 0 ;
	if(fd == 0){
		if(logi_open() < 0) return -1;
	}
	count = pwrite(fd, buffer, length, address);
	return count ;
}
int logi_read(unsigned char * buffer, unsigned int length, unsigned int address){
	int count = 0 ;
	if(fd == 0){
		if(logi_open() < 0) return -1;
	}
	count = pread(fd, buffer, length, address);
	return count ;
}

