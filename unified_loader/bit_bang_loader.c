#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "bit_bang_loader.h"


#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_REG(g) *(gpio+(((g)/10)))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_LEV *(gpio+13) // clears bits which are 1 ignores bits which are 0


#define INIT 23
#define PROG 24
#define DONE 25

void *gpio_map;

// I/O access
volatile unsigned *gpio;
unsigned cfg_save[3] ;

void clear_bb_progb(){
	GPIO_CLR = 1<<PROG;
}
void set_bb_progb()(){
	GPIO_SET = 1<<PROG;
}
char get_bb_done(){
	return ((GPIO_LEV >> DONE) & 0x01) ;
}
char get_bb_init(){
	return ((GPIO_LEV >> INIT) & 0x01) ;
}



int init_bb_loader(){
	unsigned int i = 0 ;
	int mem_fd ;
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		printf("can't open /dev/mem \n");
		exit(EXIT_FAILURE);
	}

	/* mmap GPIO */
	gpio_map = mmap(
	NULL,             //Any adddress in our space will do
	BLOCK_SIZE,       //Map length
	PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
	MAP_SHARED,       //Shared with other processes
	mem_fd,           //File to map
	GPIO_BASE         //Offset to GPIO peripheral
	);

	close(mem_fd); //No need to keep mem_fd open after mmap

	if (gpio_map == MAP_FAILED) {
		printf("mmap error %04x\n", (unsigned int) gpio_map);//errno also set!
		exit(EXIT_FAILURE);
	}

	// Always use volatile pointer!
	gpio = (volatile unsigned *)gpio_map;

	for(i = 0; i < 5 ; i ++){
		switch(i){
			case 0:
				cfg_save[i] = GPIO_REG(SCLK);
				break ;
			case 1:
				cfg_save[i] = GPIO_REG(MOSI);
				break ;
			case 2:	
				cfg_save[i] = GPIO_REG(INIT);
				break ;
			case 3:
				cfg_save[i] = GPIO_REG(PROG);
				break ;
			case 4:
				cfg_save[i] = GPIO_REG(DONE);
				break ;
			default: 
				break ;
		};
		
	}

	INP_GPIO(INIT);
	INP_GPIO(PROG);
	INP_GPIO(DONE);	

	OUT_GPIO(PROG);
	return 0 ;
}


void close_bb_loader(){
	for(i = 0; i < 3 ; i ++){
		switch(i){
			case 0:	
				GPIO_REG(INIT) = cfg_save[i] ;
				break ;
			case 1:
				GPIO_REG(PROG) = cfg_save[i] ;
				break ;
			case 2:
				GPIO_REG(DONE) = cfg_save[i] ;
				break ;
			default: 
				break ;
		};
		
	}
}


