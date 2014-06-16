#include "LPC13xx.h"
#include "integer.h"
#ifndef _FLASH_H
#define _FLASH_H


void InitFLASH();
void M25PXX_RDID(BYTE *ID);
void M25PXX_WRSR(BYTE StatusReg);
BYTE M25PXX_RDSR();
void M25PXX_WRDI();
void M25PXX_WREN();
void BlockUntilFlashOperationComplete();
void MP25PE_Read(DWORD Page,DWORD Length, BYTE *Data);
void MP25PE_PageProgram(DWORD Page,DWORD Length,BYTE *Data);
void MP25PE_PageErase(DWORD Page);




#endif
