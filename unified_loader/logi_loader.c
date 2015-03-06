#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h> 
#include <math.h>
#include "logi_loader.h"

//PROCESS
#define CONFIG_CYCLES 1
#define SSI_DELAY 1



//LOADER
struct i2c_loader_struct * fpga_loader ;

//FILE DESCRIPTORS
int i2c_fd ;
int spi_fd ;
int fd;


//SPI DEVICE SETUP
#define SPI_MAX_LENGTH 4096
static unsigned int spi_mode = 0 ;
static unsigned int spi_bits = 8 ;
static unsigned long spi_speed = 8000000UL ;
static unsigned int delay = 0;

//CONFIG BIT ARRAY
unsigned char configBits[1024*1024*4], configDummy[1024*1024*4];

//PROTOTYPES
char checkDone();
char checkInit();
void __delay_cycles(unsigned long cycles);
int init_i2c(char * path, char expander_addr);
int init_spi(char * path);
char serialConfig(unsigned char * buffer, unsigned int length);
void serialConfigWriteByte(unsigned char val);

//PRINT COMMAND LINE HELP OPTIONS
void printHelp(){
	printf("Usage : logi_loader -[r|h] <bitfile> \n");
	printf("-r	will put the FPGA in reset state (lower power consumption)\n");
	printf("-h      will print the help \n");
}

//DELAY N CYCLES
void __delay_cycles(unsigned long cycles){
	while(cycles != 0){
		cycles -- ;	
	}
}

//MIN FUNTION
static inline unsigned int min(unsigned int a, unsigned int b){
	if(a < b) return a ;
	return b ;
}

//SET PIN ON I2C EXPANDER
static inline void i2c_set_pin(struct i2c_loader_struct * ldr_ptr, unsigned char pin, unsigned char val)
{
	unsigned char i2c_buffer[2];

	if (ioctl(i2c_fd, I2C_SLAVE, ldr_ptr->expander_address) < 0) {
		return ; 
	}

	i2c_buffer[0] = ldr_ptr->expander_out;
	write(i2c_fd, i2c_buffer, 1);
	read(i2c_fd, &i2c_buffer[1], 1);

	if (val == 1) {
		i2c_buffer[1] |= (1 << pin);
	} else {
		i2c_buffer[1] &= ~(1 << pin);
	}

	write(i2c_fd, i2c_buffer, 2);
}

//GET PIN ON I2C EXPANDER
static inline unsigned char i2c_get_pin(struct i2c_loader_struct * ldr_ptr, unsigned char pin)
{
	unsigned char i2c_buffer;

	if (ioctl(i2c_fd, I2C_SLAVE, ldr_ptr->expander_address) < 0) {
		return ;
	}
		
	i2c_buffer = ldr_ptr->expander_in;
	write(i2c_fd, &i2c_buffer, 1);
	read(i2c_fd, &i2c_buffer, 1);

	return ((i2c_buffer >> pin) & 0x01);
}

//INIT SPI DEVICE
int init_spi(char * path){
	int ret ;
	spi_fd = open(path, O_RDWR);
	if (spi_fd < 0){
		printf("can't open SPI spi_device\n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (ret == -1){
		printf("can't set spi mode \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
	if (ret == -1){
		printf("can't get spi mode \n ");
		return -1 ;
	}

	/*
	 * bits per word
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
	if (ret == -1){
		printf("can't set bits per word \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits);
	if (ret == -1){
		printf("can't get bits per word \n");
		return -1 ;
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		printf("can't set max speed hz \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		printf("can't get max speed hz \n");
		return -1 ;
	}
	

	return 1;
}


//INIT I2C DEVICE
int init_i2c(char * path, char expander_addr){
  i2c_fd = open(path, O_RDWR);
  if (i2c_fd < 0) {
	// ERROR HANDLING; you can check errno to see what went wrong 
	printf("could not open I2C device : %s!\n", path);
	close(i2c_fd);
   	return -1 ;
  }
  if (ioctl(i2c_fd, I2C_SLAVE, expander_addr) < 0){
		printf("I2C communication error ! \n");     
		close(i2c_fd);
		return -1 ;
  }
  return 0 ;
}


//RESET THE FPGA - COMMNAD LINE OPTION
void resetFPGA(){
        unsigned char * bitBuffer;
        unsigned char i2c_buffer[4];

        i2c_buffer[0] = fpga_loader->expander_cfg;
	i2c_buffer[1] = 0xFF;
	i2c_buffer[1] &= ~((1 << fpga_loader->prog_pin) | (1 << fpga_loader->mode1_pin) );
	if(fpga_loader->mux_oen_pin >= 0){
		i2c_buffer[1] &= ~(1 << fpga_loader->mux_oen_pin);
	}
	
        if (ioctl(i2c_fd, I2C_SLAVE, fpga_loader->expander_address) < 0){
                printf("I2C communication error ! \n");     
        }
        write(i2c_fd, i2c_buffer, 2); // set SSI_PROG,MODE1, MUX_OEn as output others as inputs
       
	i2c_set_pin(fpga_loader, fpga_loader->mode1_pin, 1);
        i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 0);
}

//CONFIGURE THE FPGA USING SLAVE SERIAL CONFIGURATION INTERFACE
char serialConfig(unsigned char * buffer, unsigned int length){
	int iCfg;
	unsigned long int i;
	unsigned long int timer = 0;
	unsigned char * bitBuffer;
	unsigned char i2c_buffer[4];
	unsigned int write_length, write_index ;
	

	//configuring inputs and outputs
	i2c_buffer[0] = fpga_loader->expander_cfg;
	i2c_buffer[1] = 0xFF;
	i2c_buffer[1] &= ~((1 << fpga_loader->prog_pin) | (1 << fpga_loader->mode1_pin) );
	if(fpga_loader->mux_oen_pin >= 0){
		i2c_buffer[1] &= ~(1 << fpga_loader->mux_oen_pin);
	}

	if (ioctl(i2c_fd, I2C_SLAVE, fpga_loader->expander_address) < 0){
		return -1 ;	
	}
	write(i2c_fd, i2c_buffer, 2); // set SSI_PROG, MODE1, MUX_OEn as output others as inputs

	/*
	//configuring pull-ups
	i2c_buffer[0] = 0x02;
	i2c_buffer[1] = 0xFF;
	if (ioctl(i2c_fd, I2C_SLAVE, I2C_IO_EXP_ADDR) < 0){
		return -1 ;	
	}
	write(i2c_fd, i2c_buffer, 2); 
	*/


	i2c_set_pin(fpga_loader, fpga_loader->mux_oen_pin, 0);
	i2c_set_pin(fpga_loader, fpga_loader->mode1_pin, 1);
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);

	__delay_cycles(100 * SSI_DELAY);

	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);
	__delay_cycles(10 * SSI_DELAY);
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 0);
	__delay_cycles(5 * SSI_DELAY);

	while (i2c_get_pin(fpga_loader, fpga_loader->init_pin) > 0 && timer < 200)
		timer++; // waiting for init pin to go down

	if (timer >= 200) {
		printf("FPGA did not answer to prog request, init pin not going low \n");
		i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);
		return -1;
	}

	timer = 0;
	__delay_cycles(5 * SSI_DELAY);
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);

	
	while (i2c_get_pin(fpga_loader, fpga_loader->init_pin) == 0 && timer < 256) { // need to find a better way ...
		timer++; // waiting for init pin to go up
	}
	

	if (timer >= 256) {
		printf("FPGA did not answer to prog request, init pin not going high \n");
		return -1;
	}

	timer = 0;
	write_length = min(length, SPI_MAX_LENGTH);
	write_index = 0 ;
	while(length > 0){
		if(write(spi_fd, &buffer[write_index], write_length) < write_length){
			printf("spi write error \n");
		}
		write_index += write_length ;
		length -= write_length ;
		write_length = min(length, SPI_MAX_LENGTH);
	}

	if (i2c_get_pin(fpga_loader, fpga_loader->done_pin) == 0) {
		printf("FPGA prog failed, done pin not going high \n");
		return -1;
	}

	i2c_buffer[0] = fpga_loader->expander_cfg;
	//i2c_buffer[1] = 0xDC;
	i2c_buffer[1] = 0xFF;
	i2c_buffer[1] &= ~((1 << fpga_loader->prog_pin) | (1 << fpga_loader->mode1_pin));
	if(fpga_loader->flash_rst_pin >= 0){
		i2c_buffer[1] &= ~(fpga_loader->flash_rst_pin);
	}
	if(fpga_loader->flash_rst_pin >= 0){
		i2c_buffer[1] &= ~(fpga_loader->flash_rst_pin);
	}
	write(i2c_fd, i2c_buffer, 2); // set all unused config pins as input (keeping mode pins and PROG as output)
	
	return length;
}


int init_loader(){
#ifdef LOGIBONE
	int success = 0;
	int retry = 0 ;
	while(!success & retry < sizeof(logi_variants) ){
		fpga_loader = logi_variants[retry] ;	
		if(init_i2c(fpga_loader->i2c_path, fpga_loader->expander_address) >= 0){
			success = 1 ;
			printf("Board variant is %s \n", fpga_loader-> name);
		}
	}
	if(retry >= sizeof(logi_variants)){
		printf("Cannot detect LOGI-BONE \n");
	 	return -1 ;
	}
#else
	fpga_loader = &logipi_r1_5_loader ; // if not logibone, board is logipi
	if(init_i2c(fpga_loader->i2c_path, fpga_loader->expander_address) < 0){
		 printf("Cannot detect LOGI-PI \n");
		 return -1 ;
	}
	printf("Board variant is %s \n", fpga_loader-> name);
#endif
	if(init_spi(fpga_loader->spi_path) < 0){
		 printf("can't open SPI bus \n");
		 return -1 ;
	}
}

//MAIN FUNCTION******************************************************
int main(int argc, char ** argv){
	char c ;
	unsigned int i = 1;
	FILE * fr;
	long start_time, end_time ;
	double diff_time ;
	struct timespec cpu_time ;
	unsigned int size = 0 ;	
	
	init_loader();
	/*
	//parse programm args
	for(i = 1 ; i < argc ; ){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case '\0': 
					i ++ ;
					break ;
				case 'r' :
					resetFPGA(); 
					closeGPIOs();
					close(i2c_fd);
					return 1 ;
					break ;
				case 'h' :
					printHelp();
					return 1 ;
					break;
				default :
					printHelp();
					return 1 ;
					break ;
			}
		}else{
			//last argument is file to load
			break ;
		}
	}
	*/
	fr = fopen (argv[i], "rb");  /* open the file for reading bytes*/
	if(fr < 0){
		printf("cannot open file %s \n", argv[1]);
		return -1 ;	
	}
	memset((void *) configBits, (int) 0, (size_t) 1024*1024);
	size = fread(configBits, 1, 1024*1024, fr);
	printf("bit file size : %d \n", size);
	
	//8*5 clock cycle more at the end of config
	if(serialConfig(configBits, size+5) < 0){
		printf("config error \n");
		exit(0);	
	}else{
		printf("config success ! \n");	
	}
	
	fclose(fr);
	close(spi_fd);
	close(i2c_fd);
	return 1;
}
