/*
 * FLASH.c
 *
 *  Created on: Feb 6, 2011
 *      Author: ehughes
 */



#include "LPC13xx.h"                        /* LPC134x definitions */
#include "FLASH.h"
#include "integer.h"

#define SSP (1<<11)
#define FLASH_CS0_LOC	0x0004

#define FLASH_CS0_ACTIVE	LPC_GPIO0->DATA&=~FLASH_CS0_LOC
#define FLASH_CS0_INACTIVE  LPC_GPIO0->DATA|=FLASH_CS0_LOC

#define FLASH_EN	FLASH_CS0_ACTIVE
#define FLASH_DIS	FLASH_CS0_INACTIVE

#define SSP_SSE 0x02
#define TNF 	0x01
#define RNE		0x04

#define SPI_WRITE_REG	LPC_SSP->DR
#define SPI_READ_REG	LPC_SSP->DR
#define WAIT_FOR_SPI	while(((LPC_SSP->SR)&RNE)==FALSE)

#define WIP 0x01

#define WREN  0x06
#define WRDI 0x04
#define RDID 0x9f
#define RDSR 0x05
#define WRSR 0x01
#define READ 0x03
#define FAST_READ 0x0B
#define PP 0x02
#define SE 0xD8
#define BE 0xC7
#define DP 0xB9
#define RES 0xAB
#define PE 0xDB
#define PW 0x0A

BYTE SPIJunk;

void InitFLASH()
{
	//Set Direction to Output
	LPC_GPIO0->DIR |= FLASH_CS0_LOC;
	//MakeSure GPIO is selected for the CS signal
	LPC_IOCON->PIO0_2 = 0;

	LPC_IOCON->PIO0_8 = 1;		//Enable the SPI on the ports!
	LPC_IOCON->PIO0_9 = 1;
	LPC_IOCON->PIO2_11 = 1;
	LPC_IOCON->SCKLOC = 1;

	LPC_SYSCON->SYSAHBCLKCTRL |=SSP; //enable clock to the SSP
    LPC_SYSCON->SSPCLKDIV = 2;
	LPC_SYSCON->PRESETCTRL |= 1;

	LPC_SSP->CPSR = 2;

	LPC_SSP->CR0 = 0x00000107;		// 0,0 mode and 8 data bits
	LPC_SSP->CR1 |= SSP_SSE;
}


//THis function acquires the identification from the M25PX device (BLocking SPI)
void M25PXX_RDID(BYTE *ID)
{
	BYTE i;

	//First Bring chips select LOw

	FLASH_EN;

	//send out ID instruction
	SPI_WRITE_REG = RDID;
	WAIT_FOR_SPI;

	i = SPI_READ_REG; //First Byte back is Junk

	//Now, send three dummy bytes

	for(i=0;i<3;i++)
	{
		SPI_WRITE_REG = 0xff;
		WAIT_FOR_SPI;
		ID[i] = SPI_READ_REG;
	}

	FLASH_DIS;

}

BYTE M25PXX_RDSR()
{
	BYTE i;
	//First Bring chip select LOw

	FLASH_EN;
	//send out read status register instruction
	SPI_WRITE_REG = RDSR;
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //First Byte back is Junk

	//Send out Dummy Byte to get status reg
	SPI_WRITE_REG = 0xFF;
	WAIT_FOR_SPI;

	FLASH_DIS;

	i = SPI_READ_REG;
	return i;
}

void M25PXX_WRSR(BYTE StatusReg)
{


	M25PXX_WREN();

	//First Bring chip select LOW
	FLASH_EN;
	//send out Write Status Reg instruction
	SPI_WRITE_REG = WRSR;

	WAIT_FOR_SPI;
    SPIJunk = SPI_READ_REG; //First Byte back is Junk

	//Send out New Status Register value
	SPI_WRITE_REG = StatusReg;
	WAIT_FOR_SPI;

	FLASH_DIS;
	SPIJunk = SPI_READ_REG;

}

void M25PXX_WREN()
{
	//First Bring chip select Low

	FLASH_EN;

	//send out Write Enable Instruction.

	SPI_WRITE_REG = WREN;
	WAIT_FOR_SPI;
	SPIJunk = SPI_READ_REG; //First Byte back is Junk

	FLASH_DIS;


}

void M25PXX_WRDI()
{

	//First Bring chip select Low
	FLASH_EN;
	//send out Write Enable INStruction.
	SPI_WRITE_REG = WRDI;
	WAIT_FOR_SPI;
	SPIJunk = SPI_READ_REG; //First Byte back is Junk

	FLASH_DIS;
}

void MP25PE_PageErase(DWORD Page)
{
	M25PXX_WREN();
	FLASH_EN;
	//send out Page Erase Instruction
	SPI_WRITE_REG = PE;
	WAIT_FOR_SPI;
    SPIJunk = SPI_READ_REG; //First Byte back is Junk

	//send out Address --> Page Aligned
	SPI_WRITE_REG = (BYTE)(Page>>16);
	WAIT_FOR_SPI;
	 SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(Page>>8);
	WAIT_FOR_SPI;
	SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(0);
	WAIT_FOR_SPI;
    SPIJunk=SPI_READ_REG; //Junk

	FLASH_DIS;

	BlockUntilFlashOperationComplete();

	M25PXX_WRDI();
}

void MP25PE_PageProgram(DWORD Page,DWORD Length,BYTE *Data)
{
	//This function assumes your page has been erased!

	DWORD i;

	M25PXX_WREN();
	FLASH_EN;

	SPI_WRITE_REG = PP;
	WAIT_FOR_SPI;
    SPIJunk = SPI_READ_REG; //First Byte back is Junk

	SPI_WRITE_REG = (BYTE)(Page>>16);
	WAIT_FOR_SPI;
	SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(Page>>8);
	WAIT_FOR_SPI;
	SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(Page);
	WAIT_FOR_SPI;
    SPIJunk=SPI_READ_REG; //Junk

    for(i=0;i<Length;i++)
	{
    	SPI_WRITE_REG = Data[i];
    	WAIT_FOR_SPI;
    	SPIJunk=SPI_READ_REG; //Junk
    }

	FLASH_DIS;

	BlockUntilFlashOperationComplete();
	M25PXX_WRDI();
}

void MP25PE_Read(DWORD Page,DWORD Length, BYTE *Data)
{
	DWORD i;

	FLASH_EN;
	//send out Page Erase Instruction
	SPI_WRITE_REG = READ;
	WAIT_FOR_SPI;
    SPIJunk = SPI_READ_REG; //First Byte back is Junk

	//send out Address
	SPI_WRITE_REG = (BYTE)(Page>>16);
	WAIT_FOR_SPI;
	SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(Page>>8);
	WAIT_FOR_SPI;
	SPIJunk=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(Page);
	WAIT_FOR_SPI;
    SPIJunk=SPI_READ_REG; //Junk

    for(i=0;i<Length;i++)
	{
    	SPI_WRITE_REG = 0x0;
    	WAIT_FOR_SPI;
    	Data[i]=(BYTE)SPI_READ_REG;
    }
	FLASH_DIS;
}

void BlockUntilFlashOperationComplete()
{
	BYTE FLASHStatusByte;
	FLASHStatusByte = M25PXX_RDSR();

	while((FLASHStatusByte&WIP) > 0)
		{
			FLASHStatusByte = M25PXX_RDSR();
		}
}



