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
#include "logilib.h"

//#define PWM_DIVIDER_ADDR 0X0010
//#define PWM_PERIOD_ADDR 0X0012
//#define PWM_DUTY_ADDR 0X0014


int wb_wr(unsigned int addr, unsigned short val){

	logi_write( &val, (unsigned int)2, addr);
	printf("address:%x Write Val:%x \r\n",addr, val);
	return(1);
}

int wb_rd(unsigned int addr){
	unsigned short read_val;

	logi_read(&read_val, (unsigned int)2, addr);
	printf("address: 0x%x   Read Value: 0x%x \r\n", addr, read_val);

	return(1);
}

int wb_wr_range(unsigned int start_addr, unsigned int stop_addr, unsigned short val){
	unsigned short i;
	for(i = start_addr; i<stop_addr; i++){
		logi_write( &val, (unsigned int)2, i);
		printf("address:%X Write Val:%x \r\n",i, val);
	}
	return(1);
}



int wb_rd_range(int start_addr, int stop_addr){
	unsigned short read_val,i;

	for(i = start_addr; i<stop_addr; i++){
		logi_read(&read_val, (unsigned int)2, i);
		printf("address: 0x%X   Read Value: 0x%X \r\n",i , read_val);
	}
	return(1);
}

//MAIN ********************************************************
int main(int argc, char ** argv){
	unsigned int i;
	unsigned int start_addr, stop_addr, addr, val;

	if(argc  < 1){
		printf("not enough arguments \n");
		return 0 ;
	}
	
	if( strcmp( argv[1], "wb_wr_range")==0 ){
		start_addr = strtol(argv[2], NULL, 0);
		stop_addr = strtol(argv[3], NULL, 0);
		val = strtol(argv[4], NULL, 0);
		printf("wb_write_range start: 0x%4x stop: 0x$4x val: 0x%4x \r\n",start_addr, stop_addr, val);
		wb_wr_range(start_addr, stop_addr, (unsigned short)val); 
	}
	else if( strcmp( argv[1], "wb_rd_range")==0 ){
		start_addr = strtol(argv[2], NULL, 0);
		stop_addr = strtol(argv[3], NULL, 0);
		printf("wb_write_range start: 0x%4x stop: 0x$4x\r\n",start_addr, stop_addr);
		wb_rd_range(start_addr, stop_addr);
	}
	else if( strcmp( argv[1], "wb_wr")==0 ){	//put the short strings last, 
		addr = strtol(argv[2], NULL, 0);
		val = strtol(argv[3], NULL, 0);
		printf("wb_write Address: 0x%4x Value: 0x%4x \r\n", addr, val);
		wb_wr( addr, (unsigned short)val); 
	}
	else if( strcmp( argv[1], "wb_rd")==0 ){	//put the short strings last, 
		addr = strtol(argv[2], NULL, 0);
		printf("wb_write Address: 0x%4x Value: 0x%4x \r\n", addr, val);
		wb_rd( addr); 
	}
	else {
		printf("!!no function match found!! \r\n");
	}

	return 0 ;
}
