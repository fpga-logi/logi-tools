#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "i2c_loader.h"


struct i2c_loader_struct * fpga_loader ;
int i2c_fd ;

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


int init_port_for_ssi(){
	unsigned char i2c_buffer[4];
	//configuring inputs and outputs
	i2c_buffer[0] = fpga_loader->expander_cfg;
	i2c_buffer[1] = 0xFF;
	i2c_buffer[1] &= ~((1 << fpga_loader->prog_pin) | (1 << fpga_loader->mode1_pin) );
	if(fpga_loader->mux_oen_pin >= 0){
		i2c_buffer[1] &= ~(1 << fpga_loader->mux_oen_pin);
	}

	if (ioctl(i2c_fd, I2C_SLAVE, fpga_loader->expander_address) < 0)
		return -1 ;	
	
	write(i2c_fd, i2c_buffer, 2); // set SSI_PROG, MODE1, MUX_OEn as output others as inputs


	i2c_set_pin(fpga_loader, fpga_loader->mux_oen_pin, 0);
	i2c_set_pin(fpga_loader, fpga_loader->mode1_pin, 1);
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);
	return 0 ;
}

int init_i2c_loader(struct i2c_loader_struct * selected_loader){
	char dummy ;
	i2c_fd = open(selected_loader->i2c_path, O_RDWR);
	if (i2c_fd < 0) {
	// ERROR HANDLING; you can check errno to see what went wrong 
		printf("could not open I2C device : %s!\n", selected_loader->i2c_path);
		close(i2c_fd);
		return -1 ;
	}
	if (ioctl(i2c_fd, I2C_SLAVE, selected_loader->expander_address) < 0){
		printf("I2C communication error ! \n");     
		close(i2c_fd);
		return -1 ;
	}

	if(read(i2c_fd, &dummy, 1) != 1){
		return -1 ;
	}
	return init_port_for_ssi() ;
}


void clear_i2c_progb(){
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 0);
}

void set_i2c_progb(){
	i2c_set_pin(fpga_loader, fpga_loader->prog_pin, 1);
}

char get_i2c_init(){
	return i2c_get_pin(fpga_loader, fpga_loader->init_pin) ;
}

char get_i2c_done(){
	return  i2c_get_pin(fpga_loader, fpga_loader->done_pin) ;
}

void close_i2c_loader(){
close(i2c_fd);
}


