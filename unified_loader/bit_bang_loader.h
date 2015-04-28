
#ifndef BIT_BANG_LOADER_H
#define BIT_BANG_LOADER_H
struct bb_loader_struct{
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


void clear_bb_progb();
void set_bb_progb();
char get_bb_done();
char get_bb_init();
int init_bb_loader();
void close_bb_loader();

#endif
