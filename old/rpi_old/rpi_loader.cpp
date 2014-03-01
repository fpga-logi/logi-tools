//
//  rpi_loader.cpp
//
//  Created by Ting Cao on 3/24/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
//  Reference:
//    BCM2835 ARM Peripherals TRM
//    http://elinux.org/RPi_Low-level_peripherals
//
#define RPI_REVISION_2

#include <sys/mman.h>
#include <fcntl.h>

#include "i2c_loader.h"

#define PAGE_SIZE (4*1024)

#define BCM2835_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2835_PERI_BASE + 0x200000) /* GPIO controller */
#define GPIO_MOSI (0x00000400)
#define GPIO_SCLK (0x00000800)

#define GPFSEL_MASK (0x07)
#define GPFSEL0_OFFSET (0)
   //FSEL0 - Revision 1 SDA0
   #define GPFSEL0_POSITION (0)
   //FSEL1 - Revision 1 SCL0
   #define GPFSEL1_POSITION (3)
   //FSEL2 - Revision 2 SDA1
   #define GPFSEL2_POSITION (6)
   //FSEL3 - Revision 2 SCL1
   #define GPFSEL3_POSITION (9)

#define GPFSEL1_OFFSET (1)
   //FSEL10 - MOSI
   #define GPFSEL10_POSITION (0)
   //FSEL11 - MOSI
   #define GPFSEL11_POSITION (3)

#define GPIOSEL_INPUT (0)
#define GPIOSEL_OUTPUT (1)
#define GPIOSEL_ATL0 (4)
#define GPIOSEL_ALL (7)

/*
   GPIO2  - SDA1, GPFSEL0 b8:6
   GPIO3  - SCL1, GPFSEL0 b11:9
   GPIO10 - MOSI, GPFSEL1 b2:0
   GPIO11 - SCLK, GPFSEL1 b5:3
 */
class RPI_I2C_Loader : public I2C_Loader
{
protected:
   static RPI_I2C_Loader* m_gInstance;
   unsigned int m_uGPFSEL0;
   unsigned int m_uGPFSEL1;
   volatile unsigned int *m_gpio;
   
#define GPIO_FSEL0 *(m_gpio)
#define GPIO_FSEL1 *(m_gpio+1)
#define GPIO_SET *(m_gpio+7)
#define GPIO_CLR *(m_gpio+10)
#define GPIO_GET *(m_gpio+13)
   
public:
   ~RPI_I2C_Loader()
   {
      try
      {
         if ( 0 != m_gpio )
         {
            // restore GPIO
            GPIO_FSEL0 = m_uGPFSEL0;
            GPIO_FSEL1 = m_uGPFSEL1;
            
            munmap( (void*)m_gpio, PAGE_SIZE );
         }
      
         if ( 0 != m_pBuffer )
         {
            free( m_pBuffer );
         }
      }
      catch (...) {}
   }

   static RPI_I2C_Loader* GetInstance()
   {
      if ( 0 == m_gInstance )
      {
         m_gInstance = new RPI_I2C_Loader(
#ifdef RPI_REVISION_2
                                          "/dev/i2c-1"
#else
                                          "/dev/i2c-0"
#endif
                                          );
      }
      return m_gInstance;
   }
   
   volatile unsigned int* GetRegister( bool set )
   {
      return m_gpio + ( set ? 7 : 10 );
   }
   // set or clear GPIO11
   virtual void SetClk( bool set = true )
   {
      volatile unsigned int *clk = GetRegister( set );
      *clk = GPIO_SCLK;
   }
   
   // set or clear GPIO10
   virtual void SetData( bool set = true )
   {
      volatile unsigned int *clk = GetRegister( set );
      *clk = GPIO_MOSI;
   }

protected:
   RPI_I2C_Loader( const char* i2c_device )
   : I2C_Loader( i2c_device ),
   m_uGPFSEL0( 0 ),
   m_uGPFSEL1( 0 ),
   m_gpio( 0 )
   {
      int fd = open( "/dev/mem", O_RDWR|O_SYNC );
      if ( 0 > fd )
      {
         perror( "make sure you have permission to access /dev/mem\n" );
         exit(-1);
      }
      void *map = mmap( 0,
                     PAGE_SIZE,
                     PROT_READ|PROT_WRITE,
                     MAP_SHARED,
                     fd,
                     ( GPIO_BASE & ~(PAGE_SIZE-1) ) );
      if ( MAP_FAILED == map )
      {
         close( fd );
         perror( "failed to mmap\n" );
         exit( -2 );
      }
      
      close( fd );
      m_gpio = (volatile unsigned int*)map;
   }
   
public:
   bool Initialize( int argc, const char* argv[] )
   {
      // 1) backup the GPIP setup to restore when the loader quits
      m_uGPFSEL0 = GPIO_FSEL0;
      m_uGPFSEL1 = GPIO_FSEL1;
      
      bool success = false;
      if ( 2 == argc )
      {
         if ( FILE* in = fopen( argv[1], "rb" ))
         {
            if ( 0 == fseek( in, 0, SEEK_END ))
            {
               long size = ftell( in );
               if ( 0 < size )
               {
                  m_uSize = (unsigned int)size;
                  rewind ( in );
                  
                  if ( 0 != ( m_pBuffer = (unsigned char*)malloc( size ) ) )
                  {
                     unsigned read_count = 0;
                     if ( m_uSize == ( read_count = fread( m_pBuffer, 1, m_uSize, in ) ) )
                     {
                        success = true;
                     }
                     else
                     {
                        cout << "only read >" << read_count << "< bytes out of >" << m_uSize << "< !" << endl;
                     }
                  }
                  else
                  {
                     cout << "failed to allocate >" << size << "< bytes of memory !" << endl;
                  }
               }
               else
               {
                  cout << "check the input file size: >" << argv[1] << "<!" << endl;
               }
            }
            else
            {
               cout << "can't read input file: >" << argv[1] << "< !" << endl;
            }
            
            fclose( in );
         }
         else
         {
            cout << "can't open input file: >" << argv[1] << "< !" << endl;
         }
      }
      
      if ( success )
      {
         // the bitstream content is read into the memory

         // initialize the peripheral

         // 2) setup GPIO for I2C operation
         // 2.1) reset GPIO for I2C first
         unsigned int data =
#ifdef RPI_REVISION_2
         ( GPIOSEL_ALL << GPFSEL2_POSITION ) |
         ( GPIOSEL_ALL << GPFSEL3_POSITION );
#else
         ( GPIOSEL_ALL << GPFSEL0_POSITION ) |
         ( GPIOSEL_ALL << GPFSEL1_POSITION );
#endif
         GPIO_FSEL0 &= ~data;
         
         // 2.2) set GPIO for I2C
         data =
#ifdef RPI_REVISION_2
         ( GPIOSEL_ATL0 << GPFSEL2_POSITION ) |
         ( GPIOSEL_ATL0 << GPFSEL3_POSITION );
#else
         ( GPIOSEL_ATL0 << GPFSEL0_POSITION ) |
         ( GPIOSEL_ATL0 << GPFSEL1_POSITION );
#endif
         GPIO_FSEL0 |= data;
         
         // 3) setup GPIO for OUTPUT operation
         // 3.1) reset GPIO for OUTPUT first
         data = ( GPIOSEL_ALL << GPFSEL10_POSITION ) |
         ( GPIOSEL_ALL << GPFSEL11_POSITION );
         GPIO_FSEL1 &= ~data;

         // 3.2) set GPIO for OUTPUT
         data = ( GPIOSEL_OUTPUT << GPFSEL10_POSITION ) |
         ( GPIOSEL_OUTPUT << GPFSEL11_POSITION );
         GPIO_FSEL1 |= data;
      }
      return success;
   }
};

RPI_I2C_Loader* RPI_I2C_Loader::m_gInstance = 0;

int main(int argc, const char * argv[])
{
   if ( RPI_I2C_Loader* loader = RPI_I2C_Loader::GetInstance() )
   {
      if ( loader->Initialize(argc, argv ) )
      {
         const char* message[] = {
            " is programmed successfully.",
            " failed to program.",
         };
         int message_idx = 0;
         if ( !loader->Load() )
         {
            message_idx = 1;
         }
         cout << argv[1] << message[message_idx] << endl;
      }
      else
      {
         cout << argv[0] << " input_bitstream" << endl;
      }
      delete loader;
   }
   return 0;
}
