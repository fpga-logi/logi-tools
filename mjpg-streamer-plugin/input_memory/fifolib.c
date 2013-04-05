#include "fifolib.h"

int fd ;
unsigned int fifo_size ;
int memory_fd;
volatile unsigned short * gpmc_pointer ;


int direct_memory_access_init(){
	int page_size ;	
	memory_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(memory_fd == -1){
		printf("error opening /dev/mem \n");
		exit(EXIT_FAILURE);
	}

	page_size = getpagesize();
	gpmc_pointer = (volatile unsigned short *) mmap(0, 8*page_size, 
	PROT_READ | 
	PROT_WRITE, 
	MAP_SHARED ,memory_fd, 
	FPGA_BASE_ADDR + FIFO_BASE_ADDR);
	if(((long) gpmc_pointer) < 0){
		printf("cannot allocate pointer on %x \n", FPGA_BASE_ADDR + FIFO_BASE_ADDR);
		return -1 ;
	}
	 
	fifo_size = gpmc_pointer[FIFO_SIZE_OFFSET] ;
	printf("fifo size is : %d",fifo_size);
	return 1 ;
}

void direct_memory_access_close(){
	close(memory_fd);
}


int fifo_open(){
	int fd = open("/dev/logibone0", O_RDWR | O_SYNC);
	if(fd == -1){
		printf("error opening /dev/logibone0 \n");
		printf("switching to user space fifo (slowest) \n");
		return direct_memory_access_init();
	}
	ioctl(fd, LOGIBONE_FIFO_MODE) ;
	return 1 ;
}

void fifo_close(){
	if(fd > 0){
		close(fd);
	}
	direct_memory_access_close();
}

int fifo_write(char * data, unsigned int count){
	if(fd > 0){
		return write(fd, data, count);	
	}
	unsigned int transferred = 0 ;
	unsigned int transfer_size = 0 ;
	char * src_addr =(char *) data;
	if(count < FIFO_BLOCK_SIZE){
		transfer_size = count ;
	}else{
		transfer_size = FIFO_BLOCK_SIZE ;
	}
	while(transferred < count){
		while(fifo_getNbFree() < transfer_size); 
		memcpy((void*) gpmc_pointer, src_addr ,transfer_size);
		src_addr += transfer_size ;
		transferred += transfer_size ;
		if((count - transferred) < FIFO_BLOCK_SIZE){
			transfer_size = count - transferred ;
		}else{
			transfer_size = FIFO_BLOCK_SIZE ;
		}
	}
	return transferred ;
}

int fifo_read(char * data, unsigned int count){
	if(fd > 0){
		return read(fd, data, count);	
	}
	unsigned int transferred = 0 ;
	unsigned int transfer_size = 0 ;
	char * trgt_addr =(char *) data;
	if(count < FIFO_BLOCK_SIZE){
		transfer_size = count ;
	}else{
		transfer_size = FIFO_BLOCK_SIZE ;
	}	
	while(transferred < count){
		while(fifo_getNbAvailable() < transfer_size); 
		memcpy(trgt_addr,(void*) gpmc_pointer, transfer_size); 
		trgt_addr += transfer_size ;
		transferred += transfer_size ;
		if((count - transferred) < FIFO_BLOCK_SIZE){
			transfer_size = (count - transferred) ;
		}else{
			transfer_size = FIFO_BLOCK_SIZE ;
		}
	}
	return transferred ;
}

void fifo_reset(){
	if(fd > 0){
		ioctl(fd, LOGIBONE_FIFO_RESET);
	}
	gpmc_pointer[FIFO_NB_AVAILABLE_A_OFFSET] = 0x00 ;
	gpmc_pointer[FIFO_NB_AVAILABLE_B_OFFSET] = 0x00 ;
}

unsigned int fifo_getNbFree(){
	if(fd > 0){
 		return ioctl(fd, LOGIBONE_FIFO_NB_FREE);
	}
	return (fifo_size - gpmc_pointer[FIFO_NB_AVAILABLE_A_OFFSET])*2 ;
}


unsigned int fifo_getNbAvailable(){
	if(fd > 0){
		return ioctl(fd, LOGIBONE_FIFO_NB_AVAILABLE);
	}
	return (gpmc_pointer[FIFO_NB_AVAILABLE_B_OFFSET]*2) ;
}

