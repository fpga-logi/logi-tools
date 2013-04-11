#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <linux/ioctl.h>

#include "jpeg_func.h"


#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

#define MEM_BASE_ADDR	 0x09000000

int min(int a, int b){
	if(a > b ){
		return b ;	
	}
	return a ;
}

int main(int argc, char ** argv){
	int fifo_fd;
	unsigned char * inputImage ;
	long start_time, end_time ;
	double diff_time ;
	struct timespec cpu_time ;

	FILE * jpeg_fd ;
	FILE * raw_file ;
	int i,j, res ;
	unsigned int pos = 0 ;
	unsigned char image_buffer[(320*240)] ; //monochrome frame buffer
	int page_size ;
	unsigned short fifo_state, fifo_data ;
	volatile unsigned short * gpmc_pointer ; 

	fifo_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fifo_fd == -1){
		printf("error opening /dev/mem \n");
		exit(EXIT_FAILURE);
	}

	page_size = getpagesize();
	printf("page size is : %d \n", page_size);

	gpmc_pointer = (volatile unsigned short *) mmap(0, page_size, 
	PROT_READ | 
	PROT_WRITE, 
	MAP_SHARED ,fifo_fd, 
	MEM_BASE_ADDR);
	if(((long) gpmc_pointer) < 0){
		printf("cannot allocate pointer on %x \n", MEM_BASE_ADDR);
		exit(EXIT_FAILURE);
	}
	printf("gpmc pointer allocated \n");
	// pointer to gpmc is allocated
	jpeg_fd  = fopen("./grabbed_frame.jpg", "w");
	if(jpeg_fd == NULL){
		perror("Error opening output file");
		exit(EXIT_FAILURE);
	}
	printf("output file openened \n");
	printf("loading input file : %s \n", argv[1]);
	res = read_jpeg_file( argv[1], &inputImage);
	if(res < 0){
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}
	
	printf("issuing reset to fifo \n");
	gpmc_pointer[6] = 0x00;
	gpmc_pointer[5] = 0x00;
	clock_gettime(CLOCK_REALTIME, &cpu_time);
	start_time = cpu_time.tv_nsec ;
	 for(i = 0 ; i < IMAGE_HEIGHT ; i ++){
                for(j = 0 ; j < IMAGE_WIDTH ; j +=8){
                        memcpy((void *) gpmc_pointer, (void *) &inputImage[(i*IMAGE_WIDTH)+j], 8);
                }
                while(gpmc_pointer[6] < ((IMAGE_WIDTH/2)-1));
                for(j = 0 ; j < IMAGE_WIDTH ; j +=8){
                        memcpy((void *) &image_buffer[(i*IMAGE_WIDTH)+j], (void *) gpmc_pointer, 8);
                }
        }
	clock_gettime(CLOCK_REALTIME, &cpu_time);
	end_time = cpu_time.tv_nsec ;
	diff_time = end_time - start_time ;
	diff_time = diff_time/1000000000 ;
	printf("transffered %d bytes in %f s : %f B/s \n", IMAGE_WIDTH * IMAGE_HEIGHT, diff_time, (IMAGE_WIDTH * IMAGE_HEIGHT)/diff_time);
	write_jpegfile(image_buffer, 320, 240, jpeg_fd, 100);
	close(fifo_fd);
	fclose(jpeg_fd);
	return 0 ;
}
