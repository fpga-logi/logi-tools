


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





// UNIFIED REGISTERS FOR LOGIBONE/LOGIPI R1.5
#define LOGI_R1_5_SSI_DONE 2
#define LOGI_R1_5_MODE1 3
#define LOGI_R1_5_SSI_PROG 0
#define LOGI_R1_5_SSI_INIT 1
#define LOGI_R1_5_FLASH_RST 5
#define LOGI_R1_5_MUX_OEn 4

//EXPANDER
#define LOGI_R1_5_I2C_IO_EXP_ADDR	0x20
#define LOGI_R1_5_I2C_IO_EXP_CONFIG_REG	0x01
#define LOGI_R1_5_I2C_IO_EXP_IN_REG	0x00
#define LOGI_R1_5_I2C_IO_EXP_OUT_REG	0x00


// REGISTERS FOR LOGIBONE R1
#define LOGIBONE_R1_SSI_DONE 3
#define LOGIBONE_R1_SSI_PROG 5
#define LOGIBONE_R1_SSI_INIT 2
#define LOGIBONE_R1_MODE0	0
#define LOGIBONE_R1_MODE1 1
#define LOGIBONE_R1_SSI_DELAY 1

//I2C
#define LOGIBONE_R1_I2C_IO_EXP_ADDR		0x24
#define LOGIBONE_R1_I2C_IO_EXP_CONFIG_REG	0x03
#define LOGIBONE_R1_I2C_IO_EXP_IN_REG		0x00
#define LOGIBONE_R1_I2C_IO_EXP_OUT_REG		0x01



struct i2c_loader_struct logibone_r1_loader = {"LOGIBONE_R1",
	"/dev/spidev1.0", 
	1,
	 "/dev/i2c-1",
	LOGIBONE_R1_I2C_IO_EXP_ADDR, 
	LOGIBONE_R1_I2C_IO_EXP_CONFIG_REG,
	LOGIBONE_R1_I2C_IO_EXP_IN_REG,
	LOGIBONE_R1_I2C_IO_EXP_OUT_REG,
	LOGIBONE_R1_MODE1,
	LOGIBONE_R1_MODE0,
	LOGIBONE_R1_SSI_DONE,
	LOGIBONE_R1_SSI_PROG,
	LOGIBONE_R1_SSI_INIT,
	-1,
	-1
	};


struct i2c_loader_struct logibone_r1_5_loader = {"LOGIBONE_R1.5",
	"/dev/spidev1.0", 
	1,
	 "/dev/i2c-1", 
	LOGI_R1_5_I2C_IO_EXP_ADDR, 
	LOGI_R1_5_I2C_IO_EXP_CONFIG_REG,
	LOGI_R1_5_I2C_IO_EXP_IN_REG,
	LOGI_R1_5_I2C_IO_EXP_OUT_REG,
	LOGI_R1_5_MODE1,
	-1,
	LOGI_R1_5_SSI_DONE,
	LOGI_R1_5_SSI_PROG,
	LOGI_R1_5_SSI_INIT,
	LOGI_R1_5_FLASH_RST,
	LOGI_R1_5_MUX_OEn
	};
struct i2c_loader_struct logipi_r1_5_loader = {"LOGIPI_R1.5",
	"/dev/spidev0.1", 
	1,
	 "/dev/i2c-1", 
	LOGI_R1_5_I2C_IO_EXP_ADDR, 
	LOGI_R1_5_I2C_IO_EXP_CONFIG_REG,
	LOGI_R1_5_I2C_IO_EXP_IN_REG,
	LOGI_R1_5_I2C_IO_EXP_OUT_REG,
	LOGI_R1_5_MODE1,
	-1,
	LOGI_R1_5_SSI_DONE,
	LOGI_R1_5_SSI_PROG,
	LOGI_R1_5_SSI_INIT,
	LOGI_R1_5_FLASH_RST,
	LOGI_R1_5_MUX_OEn
	};


struct i2c_loader_struct * logi_variants [] = {&logibone_r1_loader, &logibone_r1_5_loader};

