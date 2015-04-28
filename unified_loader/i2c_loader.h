#ifndef I2C_LOADER_H
#define I2C_LOADER_H


struct i2c_loader_struct{
	char * name ;
	char * spi_path ;
	char has_expander ;
	char * i2c_path ;
	char expander_address ;
	char expander_cfg ;
	char expander_in ;
	char expander_out ;
	char mode1_pin;
	char mode0_pin;
	char done_pin;
	char prog_pin;
	char init_pin;
	char flash_rst_pin;
	char mux_oen_pin;
};


int init_i2c_loader(struct i2c_loader_struct * selected_loader);
void clear_i2c_progb();
void set_i2c_progb();
char get_i2c_init();
char get_i2c_done();
void close_i2c_loader();

#endif
