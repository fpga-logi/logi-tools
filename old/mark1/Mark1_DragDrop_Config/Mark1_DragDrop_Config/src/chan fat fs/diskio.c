/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "FLASH.h"




//******************************************************************************************************************
// Sector Access functions for CHAN FatFs
//******************************************************************************************************************

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res;

    switch(ctrl)
    {
        case CTRL_SYNC:
             res = RES_OK;
        break;

        case GET_SECTOR_SIZE:
              res = RES_OK;
            *(WORD *)buff = 512;
        break;

        case GET_SECTOR_COUNT:
            res = RES_OK;
           *(DWORD *)buff = (WORD)4096;
        break;

        case GET_BLOCK_SIZE:
         res = RES_OK;
          *(DWORD *)buff = 1;
        break;

        default:
        res = RES_OK;
        break;
    }
    return res;
}

DSTATUS disk_initialize(BYTE Drive) {

    return 0;
}

DRESULT disk_write(BYTE Drive,const BYTE * Buffer, DWORD SectorNumber, BYTE SectorCount)
{
    BYTE i;

    for(i=0;i<SectorCount;i++)
    {

    	MP25PE_PageErase(SectorNumber*512);
    	MP25PE_PageErase((SectorNumber*512)+256);
    	MP25PE_PageProgram(SectorNumber*512,256,Buffer);
    	Buffer+=256;
    	MP25PE_PageProgram((SectorNumber*512)+256,256,Buffer);
    	Buffer+=256;


    }
    return RES_OK;
}




DRESULT disk_read(BYTE Drive, BYTE * Buffer,DWORD SectorNumber, BYTE SectorCount)
{
    BYTE i;
    for(i=0;i<SectorCount;i++)
    {

    	 MP25PE_Read(SectorNumber*512,512, Buffer);

        Buffer+=512;
    }
     return RES_OK;
}


DWORD get_fattime(void)
{
  return 0;
}

DSTATUS disk_status(BYTE Drive)
{
    return 0;
}
