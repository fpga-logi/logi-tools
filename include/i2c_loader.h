//
//  i2c_loader.h
//  rpi_loader
//
//  Created by Ting Cao on 3/24/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
#ifndef I2C_LOADER_H
#define I2C_LOADER_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <string>
using namespace std;

extern "C" {
#include "smbus.h"
}

#define I2C_SLAVE_ADDR (0x70)
#ifdef __APPLE__
#define I2C_SLAVE (100)
#endif
typedef enum
{
   I2C_CFG_CMD_PASSIVE_MODE =	1,    //command to go into passive mode
   I2C_CFG_CMD_LPC_CFG_MODE,        //command to go have LPC configure the fpga
   I2C_CFG_CMD_DONE,                //did the done bit go high?
   I2C_CFG_CMD_LPC_CFG,             //reconfigure the fpga
   I2C_CFG_CMD_SLAVE_CFG_INIT,      //reconfigure the fpga
   I2C_CFG_CMD_SLAVE_CFG_INIT_STAT, //reconfigure the fpga
}
I2C_CFG_CMD;

#define I2C_SUCCESS (0x100)

class I2C_Loader
{
public:

   virtual void SetClk( bool set = true ) = 0;
   virtual void SetData( bool set = true ) = 0;
   
   virtual bool SendCommand( int fd, I2C_CFG_CMD cmd )
   {
      int status = i2c_smbus_write_byte( fd, cmd );
      if ( 0 > status )
      {
         perror( "Failed to send I2C command" );
         cout << "Please make sure MARK-1 is connected!" << endl;
         return false;
      }
      
      return true;
   }
   
   virtual bool ReadCommand( int fd, I2C_CFG_CMD cmd, unsigned short* data )
   {
      int status = i2c_smbus_read_word_data( fd, cmd );
      if ( 0 > status )
      {
         perror( "Failed to read I2C");
         return false;
      }
      
      *data = (unsigned short)status;
      return true;
   }
   
   virtual bool Load()
   {
      int i2c = open( m_sDevice.c_str(), O_RDWR );
      if ( 0 > i2c )
      {
         cout << "failed to open I2C bus at: " << m_sDevice << ", make sure I2C is enabled and you have root access." << endl;
         return false;
      }
      int i2c_address = I2C_SLAVE_ADDR;
      if ( 0 > ioctl( i2c, I2C_SLAVE, i2c_address ) )
      {
         cout << "failed to acquire I2C bus access, make sure you have root access." << endl;
         close( i2c );
         return false;
      }
      
      SetClk( false );
      
      bool success = false;
      if ( SendCommand( i2c, I2C_CFG_CMD_SLAVE_CFG_INIT ) )
      {
         usleep( 10*1000 );
         unsigned short status;
         if ( ReadCommand( i2c, I2C_CFG_CMD_SLAVE_CFG_INIT_STAT, &status ) )
         {
            if ( I2C_SUCCESS == ( I2C_SUCCESS & status ) )
            {
               unsigned char* ptr = m_pBuffer;
               for( unsigned int i=0; i<m_uSize; i++ )
               {
                  unsigned char data = *ptr++;
                  for( int bit=7; bit>=0; bit--)
                  {
                     SetClk( false );
                     // test MSB
                     SetData( 0 != ( data & 0x80 ) );
                     // clock in the data
                     SetClk();
                     // move to the next MSB
                     data <<= 1;
                  }
               }
               
               // Dummy operations
               for( unsigned int i=0; i<10; i++ )
               {
                  SetClk( false );
                  usleep( 1000 );
                  SetClk( true );
               }
               SetClk( false );
               SetData( false );
               
               usleep ( 10*1000 );
               if ( ReadCommand( i2c, I2C_CFG_CMD_DONE, &status ) )
               {
                  /*
                  if ( I2C_SUCCESS == ( I2C_SUCCESS & status ) )
                  {
                     success = true;
                  }
                  else
                  {
                     printf( "I2C_CFG_CMD_DONE: 0x%04x\n", status );
                  }
                   */
                  if ( I2C_SUCCESS != ( I2C_SUCCESS & status ) )
                  {
                     printf( "oops, CFG_DONE is not asserted: 0x%04x\n", status );
                  }
                  success = true;
               }
            }
            else
            {
               printf( "I2C_CFG_CMD_SLAVE_CFG_INIT_STAT: 0x%04x\n", status );
            }
         }
      }
      close( i2c );
      return success;
   }
   
protected:
   I2C_Loader( const char* i2c_device )
   : m_sDevice( i2c_device ),
   m_pBuffer( 0 ),
   m_uSize( 0 )
   {
   }
   
   string m_sDevice;
   unsigned char* m_pBuffer;
   unsigned int m_uSize;
   
public:
   static I2C_Loader* m_gInstance;
};

#endif   //I2C_LOADER_H
