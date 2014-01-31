#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include "i2c-dev.h"

#define CONFIG_CYCLES 1


#define MMAP_OFFSET (0x44C00000)
#define MMAP_SIZE   (0x481AEFFF-MMAP_OFFSET)

#define GPIO0 		(0x44E07000)
#define GPIO1		(0x4804C000)
#define GPIO2		(0x481AC000)
#define GPIO3		(0x481AE000)

#define GPIO_CLEARDATAOUT (0x190)
#define GPIO_SETDATAOUT   (0x194)
#define GPIO_OE			      (0x134)
#define GPIO_DATAOUT      (0x13C)
#define GPIO_DATAIN       (0x138)

#define MARK1_ADDR 0x70

int i2c_fd ;
char isMark1 = 0 ;
int fd;
static volatile uint32_t *map;

unsigned char configBits[1024*1024*4];


char initGPIOs();
void closeGPIOs();
void clearProgramm();
void setProgramm();
char checkDone();
char checkInit();
inline void setClk();
inline void clearClk();
inline void setDout();
inline void clearDout();
void __delay_cycles(unsigned long cycles);
char write_mark1_reg(unsigned char add);
char read_mark1_word(unsigned char add, unsigned short * vals);
int init_i2c(int nb);
char probeMark1(void);

char serialConfig(unsigned char * buffer, unsigned int length);

void serialConfigWriteByte(unsigned char val);


void __delay_cycles(unsigned long cycles){
	while(cycles != 0){
		cycles -- ;	
	}
}


char probeMark1(void){
	unsigned short res ;
	read_mark1_word(0x06, &res);
	if(res < 0 || (res & 0xFF) != 0xE1){
		return 0 ;	
	}
	printf("Mark1 found ! \n");
	return 1 ;
}

char write_mark1_reg(unsigned char add){
    int resp = 0 ;
    if (ioctl(i2c_fd, I2C_SLAVE, MARK1_ADDR) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
	printf("Wrong device addr \n");
        exit(1);
    }
    resp = i2c_smbus_write_byte(i2c_fd, add);
    if (resp < 0) {
    	printf("error accessing i2c bus , returned :%d \n", resp);       
    }
    return resp;
}

char read_mark1_word(unsigned char add, unsigned short * val){
    int res = 0 ;
    if (ioctl(i2c_fd, I2C_SLAVE, MARK1_ADDR) < 0) {
	printf("Wrong device addr \n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    *val = i2c_smbus_read_word_data(i2c_fd, add);
    return *val;
}



int init_i2c(int nb){
  char filename[20];   
  int file ;   
  snprintf(filename, 19, "/dev/i2c-%d", nb);
  i2c_fd = open(filename, O_RDWR);
  if (i2c_fd < 0) {
   	return -1 ;
  /* ERROR HANDLING; you can check errno to see what went wrong */
  }
}


char initGPIOs(){
	fd = open("/dev/mem", O_RDWR);
	if(fd == -1) {
		perror("Unable to open /dev/mem");
		exit(EXIT_FAILURE);
	}
	map = (uint32_t*)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);
	if(map == MAP_FAILED) {
		close(fd);
		perror("Unable to map /dev/mem");
		exit(EXIT_FAILURE);
	}
}

void closeGPIOs(){
	close(fd);
}

void clearProgramm(){
	//map[(GPIO2-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<12);
	if(isMark1){
		write_mark1_reg(0x05);
	}else{
		map[(GPIO2-MMAP_OFFSET+GPIO_DATAOUT)/4] &= ~(1<<12);
	}
}
void setProgramm(){
	//map[(GPIO2-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<12);
	if(!isMark1){
	map[(GPIO2-MMAP_OFFSET+GPIO_DATAOUT)/4] |= (1<<12);
	}
}
char checkDone(){
	if(isMark1){
		unsigned short int val ;
		if(read_mark1_word(0x03, &val) > 0){
			if(val  == 0x00e1){
				printf("Done ok !\n");
				return 1 ;		
			}else{
				return 0 ;		
			}
		}else{
			return -1;
		}
	}else{
		return (map[(GPIO2-MMAP_OFFSET+GPIO_DATAIN)/4] & (1<<8))>>8;
	}
}
char checkInit(){
	if(isMark1){
		unsigned short int val ;
		if(read_mark1_word(0x06, &val) > 0){
			if(val == 0x01e1){
				printf("Init ok !\n");
				return 0 ;		
			}else{
				return 1 ;		
			}
		}else{
			return 1;
		}
	}else{
		return (map[(GPIO2-MMAP_OFFSET+GPIO_DATAIN)/4] & (1<<10))>>10;
	}
}
inline void setClk(){
	//map[(GPIO0-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<2);
	map[(GPIO0-MMAP_OFFSET+GPIO_DATAOUT)/4] |= (1<<2);
}
inline void clearClk(){
	//map[(GPIO0-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<2);
	map[(GPIO0-MMAP_OFFSET+GPIO_DATAOUT)/4] &= ~(1<<2);
}
inline void setDout(){
	//map[(GPIO0-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<4);
	map[(GPIO0-MMAP_OFFSET+GPIO_DATAOUT)/4] |= (1<<4);
}
inline void clearDout(){
	//map[(GPIO0-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<4);
	map[(GPIO0-MMAP_OFFSET+GPIO_DATAOUT)/4] &= ~(1<<4);
}


char serialConfig(unsigned char * buffer, unsigned int length){
	unsigned long int i ;
	unsigned long int timer = 0;
	clearClk();
	setProgramm();
	__delay_cycles(10*CONFIG_CYCLES);	
	clearProgramm();
	__delay_cycles(5*CONFIG_CYCLES);		
	while(checkInit() > 0 && timer < 200) timer ++; // waiting for init pin to go down
	if(timer >= 200){
		 printf("Init pin not going down !");
		 setProgramm();
		 return -1;	
	}
	timer = 0;
	__delay_cycles(5*CONFIG_CYCLES);
	if(!isMark1){
		setProgramm();
		while(checkInit() == 0 && timer < 0xFFFFFF){
			 timer ++; // waiting for init pin to go up
		}
		if(timer >= 0xFFFFFF){
			 printf("Init pin not going high ! \n");	
			 return -1;
		}
	}
	timer = 0;
	printf("Starting configuration ! \n");
	for(i = 0 ; i < length ; i ++){
		serialConfigWriteByte(buffer[i]);	
	}
	while(timer < 50){
		clearClk();
		__delay_cycles(CONFIG_CYCLES);
		setClk();	
		timer ++ ;
	}
	clearClk();
	clearDout();
	if(!checkDone() && timer >= 255){
		 printf("Done pin not going high ! \n");
		 return -1;	
	}
	return 1 ;
}

void serialConfigWriteByte(unsigned char val){
		unsigned char i = 0 ;
		unsigned char valBuf = val ;
		for(i = 0 ; i < 8 ; i ++){
			if((valBuf & 0x80) != 0){
				clearClk(); // clk down
				setDout();
			}else{
				clearClk();
				clearDout();
			}
			valBuf = (valBuf << 1);
			//__delay_cycles(CONFIG_CYCLES);
			setClk();
			//__delay_cycles(CONFIG_CYCLES);		
		}
}


int main(int argc, char ** argv){
	char c ;
	FILE * fr;
	long start_time, end_time ;
	double diff_time ;
	struct timespec cpu_time ;
	unsigned int size = 0 ;	
	initGPIOs();
	if(init_i2c(3) < 0){	
		isMark1 = 0 ;
	}else{
		isMark1 = probeMark1();
	}
	if(!isMark1){
		close(i2c_fd);	
	}
	fr = fopen (argv[1], "rb");  /* open the file for reading bytes*/
	if(fr < 0){
		printf("cannot open file %s \n", argv[1]);	
	}
	size = fread(configBits, 1, 1024*1024, fr);
	printf("bit file size : %d \n", size);
	//clock_gettime(CLOCK_REALTIME, &cpu_time);
	//start_time = cpu_time.tv_sec ;
	if(serialConfig(configBits, size) < 0){
		printf("config error \n");
		exit(0);	
	}else{
		printf("config success ! \n");	
	}
	//clock_gettime(CLOCK_REALTIME, &cpu_time);
	//end_time = cpu_time.tv_sec ;
	//diff_time = end_time - start_time ;
	//diff_time = diff_time/1000000 ;
	//printf("Configuration took %fs \n", diff_time);
	closeGPIOs();
	fclose(fr);
	if(isMark1){
		close(i2c_fd);
	}
	return 1;
}
