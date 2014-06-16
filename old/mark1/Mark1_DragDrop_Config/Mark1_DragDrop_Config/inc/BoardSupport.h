#ifndef _BOARD_SUPPORT_H
#define _BOARD_SUPPORT_H

//PIN LOCATION DEFINES
#define LED1_LOC				(1<<0)	//PIO3_0
#define LED0_LOC				(1<<3)	//PIO2_3

#define PB_LOC 					(1<<1)	//PIO0_1

#define CFG_INIT_B_LOC   		(1<<7)	//PIO0_7
#define CFG_DIN_LOC				(1<<2)	//PIO3_2
#define CFG_CCLK_LOC			(1<<1)	//PIO3_1
#define CFG_PROG_B_LOC   		(1<<9)	//PIO1_9
#define CFG_DONE_LOC			(1<<8)	//PIO1_8
#define	CFG_FLASH_RST_LOC		(1<<1)	//PIO1_1	- used to reset fpga cfg flash if needed - default hiz = input to no conflict
#define CFG_MODE1_LOC			(1<<8)	//PIO2_8
#define CFG_FLASH_CS_LOC		(1<<9)	//PIO2_9	- use the control CS pin of the fpga flash chip. -  default - hiz = input
#define CFG_MUX_SEL_LOC			(1<<2)	//PIO2_2	- select the mux inputs for the fpga config - select LPC config or PI/BONE slave conifg - default hiz input, pulled up to default mode = lpc config

#define FPGA_SUSPEND_LOC		(1<<3)	//PIO3_3
#define FPGA_CS_LOC				(1<<11)	//PIO0_11
#define FTDI_RST_LOC			(1<<1)	//PIO2_1
#define MASTER_SENSE_LOC		(1<<10)	//PIO2_10	- detect PI/BONE connected

#define SCL_LOC					(1<<4)//PIO0_4	//
#define SDA_LOC					(1<<5)//PIO0_5	//

//PIN USAGE DEFINES
#define PB_ACTIVE				(0)		//Push buttons active level
#define PB_STATE				(LPC_GPIO0->DATA & PB_LOC)	//PUSH BUTTON LOGIC LEVEL


#define MASTER_SENSE_STATE			(LPC_GPIO2->DATA & MASTER_SENSE_LOC) 	//GET PORTPIN VALUE

#define FTDI_RST_ACTIVE				(LPC_GPIO2->DATA	&= ~FTDI_RST_LOC)		//LOW
#define FTDI_RST_INACTIVE			LPC_GPIO2->DATA	|= FTDI_RST_LOC			//HIGH

#define FPGA_CS_ACTIVE				(LPC_GPIO0->DATA &= ~FPGA_CS_LOC)			//LOW
#define FPGA_CS_INACTIVE			(LPC_GPIO0->DATA |= FPGA_CS_LOC)			//HIGH
#define FPGA_SUSPEND_ACTIVE			(LPC_GPIO3->DATA |= FPGA_SUSPEND_LOC)
#define FPGA_SUSPEND_INACTIVE		(LPC_GPIO3->DATA &= ~FPGA_SUSPEND_LOC)

#define LED1_OFF					(LPC_GPIO3->DATA &= ~LED1_LOC)
#define LED1_ON			    		(LPC_GPIO3->DATA |= LED1_LOC)
#define LED1_TOGGLE		    		(LPC_GPIO3->DATA ^= LED1_LOC)
#define LED0_OFF					(LPC_GPIO2->DATA &= ~LED0_LOC)
#define LED0_ON			    		(LPC_GPIO2->DATA |= LED0_LOC)
#define LED0_TOGGLE		    		(LPC_GPIO2->DATA ^= LED0_LOC)

#define CFG_PROG_B_ACTIVE			(LPC_GPIO1->DATA &= ~CFG_PROG_B_LOC)		//CLEAR the PROG_B_PIN
#define CFG_PROG_B_INACTIVE			(LPC_GPIO1->DATA |= CFG_PROG_B_LOC)		//SET the PROG_B_PIN
#define CFG_DIN_ACTIVE				(LPC_GPIO3->DATA |= CFG_DIN_LOC)
#define CFG_DIN_INACTIVE			(LPC_GPIO3->DATA &= ~CFG_DIN_LOC)
#define CFG_CCLK_ACTIVE				(LPC_GPIO3->DATA |= CFG_CCLK_LOC)
#define CFG_CCLK_INACTIVE			(LPC_GPIO3->DATA &= ~CFG_CCLK_LOC)
#define CFG_DONE_STATE				(LPC_GPIO1->DATA & CFG_DONE_LOC)
#define CFG_INIT_B_STATE			(LPC_GPIO0->DATA & CFG_INIT_B_LOC)

#define CFG_FLASH_CS_ACTIVE			LPC_GPIO2->DATA &= ~CFG_FLASH_CS_LOC	//LOW
#define CFG_FLASH_CS_INACTIVE		LPC_GPIO2->DATA |= CFG_FLASH_CS_LOC		//HIGH
#define CFG_FLASH_RST_ACTIVE		LPC_GPIO1->DATA &= ~CFG_FLASH_RST_LOC	//LOW
#define CFG_FLASH_RST_INACTIVE		LPC_GPIO1->DATA |= CFG_FLASH_RST_LOC	//HIGH
#define CFG_MUX_LPC_CTL					LPC_GPIO2->DATA |= CFG_MUX_SEL_LOC		//HIGH - select LPC as much to configure FPGA
#define CFG_MUX_MASTER_CTL				LPC_GPIO2->DATA &= ~CFG_MUX_SEL_LOC		//LOW - select Pi/Bone as much to configure FPGA
#define CFG_MODE1_MASTER			LPC_GPIO2->DATA &= ~CFG_MODE1_LOC		//LOW
#define CFG_MODE1_SLAVE				LPC_GPIO2->DATA |= CFG_MODE1_LOC		//HIGH

//PROTOTYPES ************************************************
void InitGPIO();
void InitGPIO_Passive_Mode( void);
void InitGPIO_LPC_Mode();

#endif

//EOF


