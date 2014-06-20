/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    mscuser.c
 *      Purpose: Mass Storage Class Custom User Module
 *      Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 *      Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "LPC13xx.h"
#include "type.h"
#include "usb.h"
#include "msccallback.h"
#include "Flash.h"

BYTE FlashBuffer[512] = {0};

const uint8_t InquiryStr[26] = "MP25PE16                ";

void MSC_MemoryRead (uint32_t offset, uint8_t dst[], uint32_t length) {
  uint32_t n;


	 MP25PE_Read(offset,length, &FlashBuffer[0]);

	 for(n=0;n<length;n++)
	 {
		 dst[n] = FlashBuffer[n];
	 }
}


/*
 *  MSC Memory Write Callback
 *   Called automatically on Memory Write Event
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_MemoryWrite (uint32_t offset, uint8_t src[], uint32_t length)
{
  uint32_t n;

  for(n=0;n<length;n++)
  {
	 FlashBuffer[n] = src[n];
 }

  //The Writes only come in 64 bytes at a time.   Out Pages need to be erased
  //in 256 byte blocks.   If A write happens to a page aligned address we will first erase
  //This assumes that All MSC are done in nice 512 byte operations that are page aligned!

  if((offset & 0xFF) == 0)
  {
	  MP25PE_PageErase(offset);
  }

  MP25PE_PageProgram(offset,length,&FlashBuffer[0]);
}
