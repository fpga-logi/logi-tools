#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#define CONFIG_CYCLES 1

#define BCM2708_PERI_BASE        0x20000000
#define BCM2836_PERI_BASE	 0x3f000000


#ifndef PI2
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#else
#define GPIO_BASE                (BCM2836_PERI_BASE + 0x200000)
#endif

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)


int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

unsigned char configBits[1024*1024*4];

unsigned cfg_save[5] ;


#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_REG(g) *(gpio+(((g)/10)))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_LEV *(gpio+13) // clears bits which are 1 ignores bits which are 0


#define SCLK 11
#define MOSI 10
#define INIT 23
#define PROG 24
#define DONE 25


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

char serialConfig(unsigned char * buffer, unsigned int length);

void serialConfigWriteByte(unsigned char val);


void __delay_cycles(unsigned long cycles){
	while(cycles != 0){
		cycles -- ;	
	}
}

char initGPIOs(){
	unsigned int i = 0 ;
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

	INP_GPIO(SCLK);
	INP_GPIO(MOSI);
	INP_GPIO(INIT);
	INP_GPIO(PROG);
	INP_GPIO(DONE);	

	OUT_GPIO(SCLK);
	OUT_GPIO(MOSI);
	OUT_GPIO(PROG);

   	
}

void closeGPIOs(){
	unsigned int i ;
	for(i = 0; i < 5 ; i ++){
		switch(i){
			case 0:
				GPIO_REG(SCLK) = cfg_save[i] ;
				break ;
			case 1:
				GPIO_REG(MOSI) = cfg_save[i] ;
				break ;
			case 2:	
				GPIO_REG(INIT) = cfg_save[i] ;
				break ;
			case 3:
				GPIO_REG(PROG) = cfg_save[i] ;
				break ;
			case 4:
				GPIO_REG(DONE) = cfg_save[i] ;
				break ;
			default: 
				break ;
		};
		
	}
}

void clearProgramm(){
	GPIO_CLR = 1<<PROG;
}
void setProgramm(){
	GPIO_SET = 1<<PROG;
}
char checkDone(){
	return ((GPIO_LEV >> DONE) & 0x01) ;
}
char checkInit(){
	return ((GPIO_LEV >> INIT) & 0x01) ;
}
inline void setClk(){
	GPIO_SET = 1<<SCLK;
	
}
inline void clearClk(){
	GPIO_CLR = 1<<SCLK;
}
inline void setDout(){
	GPIO_SET = 1<<MOSI;
}
inline void clearDout(){
	GPIO_CLR = 1<<MOSI;
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
	setProgramm();
	while(checkInit() == 0 && timer < 0xFFFFFF){
		 timer ++; // waiting for init pin to go up
	}
	if(timer >= 0xFFFFFF){
		 printf("Init pin not going high ! \n");	
		 return -1;
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
	if(argc < 2){
		printf("Need file path \n");
		exit(EXIT_FAILURE);
	}
	initGPIOs();
	printf("gpio configured \n");
	fr = fopen (argv[1], "rb");  /* open the file for reading bytes*/
	if(fr == 0){
		closeGPIOs();
		printf("cannot open file %s \n", argv[1]);
		exit(EXIT_FAILURE);	
	}
	printf("file %s opened \n", argv[1]);
	size = fread(configBits, 1, 1024*1024, fr);
	printf("bit file size : %d \n", size);
	//clock_gettime(CLOCK_REALTIME, &cpu_time);
	if(serialConfig(configBits, size) < 0){
		printf("config error \n");
		closeGPIOs();
		fclose(fr);
		exit(EXIT_FAILURE);	
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
	return 1;
}
