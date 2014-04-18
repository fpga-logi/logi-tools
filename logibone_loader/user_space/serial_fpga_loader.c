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

#define CONFIG_CYCLES 1


#define SSI_DONE 3
#define SSI_PROG 5
#define SSI_INIT 2
#define MODE0	0
#define MODE1 1
#define SSI_DELAY 1

//I2C
#define I2C_IO_EXP_ADDR	0x24

#define I2C_IO_EXP_CONFIG_REG	0x03
#define I2C_IO_EXP_IN_REG	0x00
#define I2C_IO_EXP_OUT_REG	0x01

#define SPI_MAX_LENGTH 4096
int i2c_fd ;
int spi_fd ;
int fd;


static const char * device = "/dev/spidev1.0";
static unsigned int mode = 0 ;
static unsigned int bits = 8 ;
static unsigned long speed = 16000000UL ;
static unsigned int delay = 0;

unsigned char configBits[1024*1024*4], configDummy[1024*1024*4];


char initGPIOs();
void closeGPIOs();
void clearProgramm();
void setProgramm();
char checkDone();
char checkInit();



void __delay_cycles(unsigned long cycles);
int init_i2c(int nb);

char serialConfig(unsigned char * buffer, unsigned int length);

void serialConfigWriteByte(unsigned char val);


void __delay_cycles(unsigned long cycles){
	while(cycles != 0){
		cycles -- ;	
	}
}


static inline unsigned int min(unsigned int a, unsigned int b){
	if(a < b) return a ;
	return b ;
}

static inline void i2c_set_pin(unsigned char pin, unsigned char val)
{
	unsigned char i2c_buffer[2];

	if (ioctl(i2c_fd, I2C_SLAVE, I2C_IO_EXP_ADDR) < 0) {
		return ; 
	}

	i2c_buffer[0] = I2C_IO_EXP_OUT_REG;
	write(i2c_fd, i2c_buffer, 1);
	read(i2c_fd, &i2c_buffer[1], 1);

	if (val == 1) {
		i2c_buffer[1] |= (1 << pin);
	} else {
		i2c_buffer[1] &= ~(1 << pin);
	}

	write(i2c_fd, i2c_buffer, 2);
}

static inline unsigned char i2c_get_pin(unsigned char pin)
{
	unsigned char i2c_buffer;

	if (ioctl(i2c_fd, I2C_SLAVE, I2C_IO_EXP_ADDR) < 0) {
		return ;
	}
		
	i2c_buffer = I2C_IO_EXP_IN_REG;
	write(i2c_fd, &i2c_buffer, 1);
	read(i2c_fd, &i2c_buffer, 1);

	return ((i2c_buffer >> pin) & 0x01);
}


int init_spi(void){
	int ret ;
	spi_fd = open(device, O_RDWR);
	if (spi_fd < 0){
		printf("can't open device\n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1){
		printf("can't set spi mode \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1){
		printf("can't get spi mode \n ");
		return -1 ;
	}

	/*
	 * bits per word
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1){
		printf("can't set bits per word \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1){
		printf("can't get bits per word \n");
		return -1 ;
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1){
		printf("can't set max speed hz \n");
		return -1 ;
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1){
		printf("can't get max speed hz \n");
		return -1 ;
	}
	

	return 1;
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
	return 1 ;
}

void closeGPIOs(){
	close(fd);
}


char serialConfig(unsigned char * buffer, unsigned int length){
	int iCfg;
	unsigned long int i;
	unsigned long int timer = 0;
	unsigned char * bitBuffer;
	unsigned char i2c_buffer[4];
	unsigned int write_length, write_index ;

	i2c_buffer[0] = I2C_IO_EXP_CONFIG_REG;
	i2c_buffer[1] = 0xFF;
	i2c_buffer[1] &= ~((1 << SSI_PROG) | (1 << MODE1) | (1 << MODE0));

	if (ioctl(i2c_fd, I2C_SLAVE, I2C_IO_EXP_ADDR) < 0){
		return -1 ;	
	}
	write(i2c_fd, i2c_buffer, 2); // set SSI_PROG, MODE0, MODE1 as output others as inputs
	
	i2c_set_pin(MODE0, 1);
	i2c_set_pin(MODE1, 1);
	i2c_set_pin(SSI_PROG, 0);

	

	i2c_set_pin(SSI_PROG, 1);
	__delay_cycles(10 * SSI_DELAY);
	i2c_set_pin(SSI_PROG, 0);
	__delay_cycles(5 * SSI_DELAY);

	while (i2c_get_pin(SSI_INIT) > 0 && timer < 200)
		timer++; // waiting for init pin to go down

	if (timer >= 200) {
		printf("FPGA did not answer to prog request, init pin not going low \n");
		i2c_set_pin(SSI_PROG, 1);
		return -1;
	}

	timer = 0;
	__delay_cycles(5 * SSI_DELAY);
	i2c_set_pin(SSI_PROG, 1);

	while (i2c_get_pin(SSI_INIT) == 0 && timer < 256) { // need to find a better way ...
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

	if (i2c_get_pin(SSI_DONE) == 0) {
		printf("FPGA prog failed, done pin not going high \n");
		return -1;
	}

	i2c_buffer[0] = I2C_IO_EXP_CONFIG_REG;
	i2c_buffer[1] = 0xDC;
	write(i2c_fd, i2c_buffer, 2); // set all unused config pins as input (keeping mode pins and PROG as output)
	
	return length;
}


int main(int argc, char ** argv){
	char c ;
	FILE * fr;
	long start_time, end_time ;
	double diff_time ;
	struct timespec cpu_time ;
	unsigned int size = 0 ;	
	initGPIOs();
	init_i2c(1);
	if(init_spi() < 0){
		 printf("cannot open spi bus \n");
		 return -1 ;
	}
	fr = fopen (argv[1], "rb");  /* open the file for reading bytes*/
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
	
	closeGPIOs();
	fclose(fr);
	close(spi_fd);
	close(i2c_fd);
	return 1;
}
