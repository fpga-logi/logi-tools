#include "fifolib.h"



int fd ;
unsigned int fifo_size ;
int memory_fd;
volatile unsigned short * gpmc_pointer ;


int direct_memory_access_init(){
	int page_size ;	
	int nb_page ;
	memory_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(memory_fd == -1){
		printf("error opening /dev/mem \n");
		exit(EXIT_FAILURE);
	}

	page_size = getpagesize();
	nb_page = 0x1FFFE/page_size ;
	gpmc_pointer = (volatile unsigned short *) mmap(0, nb_page*page_size, 
	PROT_READ | 
	PROT_WRITE, 
	MAP_SHARED ,memory_fd, 
	FPGA_BASE_ADDR);
	if(((long) gpmc_pointer) < 0){
		printf("cannot allocate pointer on %x \n", FPGA_BASE_ADDR);
		return -1 ;
	}
	return 1 ;
}

void direct_memory_access_close(){
	close(memory_fd);
}


int fifo_open(unsigned char id){
	if(id >= MAX_FIFO_NB) return -1 ;
	fifo_array[id].id = id ;
	fifo_array[id].address = id * FIFO_SPACING ;
	fifo_array[id].open = -1 ;
	int fd = open("/dev/logibone0", O_RDWR | O_SYNC);
	if(fd == -1){
		int ret ;		
		printf("error opening /dev/logibone0 \n");
		printf("switching to user space fifo (slowest) \n");
		if(memory_fd == 0){
			ret = direct_memory_access_init();
		}else{
			ret = 1 ;		
		}
		if(ret > 0){
			 fifo_array[id].size = fifo_getSize(id) ;
			 fifo_array[id].open = 1 ;
			 //printf("fifo size is : %d \n",fifo_array[id].size);
		}
		return ret ;
	}
	ioctl(fd, LOGIBONE_FIFO_MODE) ;
	return 1 ;
}

void fifo_close(unsigned char id){
	if(fd > 0){
		close(fd);
	}
	fifo_array[id].open = -1 ;
	if(id == 0){
		direct_memory_access_close();
	}
}

int fifo_write(unsigned char id, unsigned char * data, unsigned int count){
	if(fd > 0){
		ioctl(fd, LOGIBONE_FIFO_MODE);
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
		while(fifo_getNbFree(id) < transfer_size); 
		memcpy((void*) &gpmc_pointer[fifo_array[id].address], src_addr ,transfer_size);
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

int fifo_read(unsigned char id, unsigned char * data, unsigned int count){
	if(fd > 0){
		ioctl(fd, LOGIBONE_FIFO_MODE);
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
		while(fifo_getNbAvailable(id) < transfer_size); 
		memcpy(trgt_addr,(void*) &gpmc_pointer[fifo_array[id].address], transfer_size); 
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

void fifo_reset(unsigned char id){
	
	if(fd > 0){
		ioctl(fd, LOGIBONE_FIFO_RESET);
	}
	gpmc_pointer[fifo_array[id].address + FIFO_NB_AVAILABLE_A_OFFSET] = 0 ;
	gpmc_pointer[fifo_array[id].address + FIFO_NB_AVAILABLE_B_OFFSET] = 0 ;
}

unsigned int fifo_getSize(unsigned char id){
	return ( gpmc_pointer[fifo_array[id].address + FIFO_SIZE_OFFSET] * 2 );
}

unsigned int fifo_getNbFree(unsigned char id){
	if(fd > 0){
 		return ioctl(fd, LOGIBONE_FIFO_NB_FREE);
	}
	return (fifo_array[id].size - (gpmc_pointer[fifo_array[id].address + FIFO_NB_AVAILABLE_A_OFFSET]*2)) ;
}


unsigned int fifo_getNbAvailable(unsigned char id){

	if(fd > 0){
		return ioctl(fd, LOGIBONE_FIFO_NB_AVAILABLE);
	}
	return (gpmc_pointer[fifo_array[id].address + FIFO_NB_AVAILABLE_B_OFFSET]*2) ;
}

unsigned int direct_write(unsigned int address, unsigned char * buffer, unsigned int length){
	memcpy((void*) &gpmc_pointer[address/2], buffer, length);
	return length ;
}
unsigned int direct_read(unsigned int address, unsigned char * buffer, unsigned int length){
	memcpy(buffer, (void*)&gpmc_pointer[address/2], length);
	return length ;
}


