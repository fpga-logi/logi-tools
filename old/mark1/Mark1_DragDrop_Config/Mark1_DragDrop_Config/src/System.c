#include "System.h"



FATFS MyFileSystem;
//USB CONFIG DATA
USB_DEV_INFO DeviceInfo;
MSC_DEVICE_INFO MscDevInfo;
ROM ** rom = (ROM **) 0x1fff1ff8;
#define     EN_TIMER32_1    (1<<10)
#define     EN_IOCON        (1<<16)
#define     EN_USBREG       (1<<14)

/*
enum  {
	STATE_SYS_STARTUP=0,
	STATE_LPC_LOADER,
	STATE_MASTER_LOADER
};
*/
volatile uint8_t SystemState = STATE_SYS_STARTUP;

static uint16_t LedBlinkTicker;
static uint16_t LedBlinkRate;
volatile uint32_t StartupDelay;

//TIMER VARIALBES
extern volatile uint32_t tick;              // tick counter
extern volatile uint32_t timers[MAX_TIMERS];    // Timer a
extern volatile int ntimers;	//number of timers used
volatile uint16_t i=0; 			//count var in systick irq



// InitSystemStateMachine() *****************************************************
void InitSystemStateMachine( void) {
    SystemState = STATE_SYS_STARTUP;
}

/*********************************************************************************
 * FUNCTION: TransitionToState
 * DESCRIPTION: Handles housekeeping between system states
 * NOTES:
 *********************************************************************************/
void TransitionToState(uint8_t NextState) {

	switch (NextState) {
		case  STATE_SYS_STARTUP:
			//TASKS TO RUN AT STARTUP
			InitGPIO();		//initialize the GPIO pins
			SysTick_Config(SystemCoreClock/1000);	//setup timer tick - Interrupt found in system.c to handle system flags etc.  tasks should be called from while loop.

			//THESE FUNCTIONS ARE FOR THE MARK-1 USB, FLASH, LOAD CONFIG.BIT
			InitFLASH();		//init flash memory.
			InitUSB_MSC();	//init usb mass storage devive drivers
			StartDel();		//init startup delay
			//SETUP FILE SYSTEM
			f_mount(0,&MyFileSystem);		//mount file system for reading config.bit file form MSD
			//FPGA_Config("config.bit");      //check to see if there is a "config.bit" file if so load it into FPGA.  Note this needs to go into while loop
												// to check for new config file loaded and reload to FPGA.  This is why there currently needs to be a power cycle
												// in order for new file to be loaded into fpga.

			break;
		case  STATE_LPC_LOADER:
			//running with lpc as loader
			InitGPIO();		//default GPIO settings
			//LED0_ON;
			SystemState = STATE_LPC_LOADER;
			break;
		case STATE_MASTER_LOADER:
			//running in slave mode, with a master loading the fpga
			InitGPIO_PASSIVE();	//setup the gpio pins as passive
			//LED0_OFF;	//turn LED1 off
			SystemState = STATE_MASTER_LOADER;
			break;
		default:
			break;

	}


}//TransitionToState


/*************************************************************
 * DESCRIPTION: startup delay
 **********************************************************8**/
void StartDel( void){
	  StartupDelay = 3000;	//not sure why this is needed
	  SetLED_BlinkRate(0);
	  while(StartupDelay>0)
	  {
	  }

}//func

// ********************************************************************
void SetLED_BlinkRate(uint16_t BR) {
	LedBlinkRate = BR;
}

// SYSTEM TICK HANDLER *************************************************
void SysTick_Handler() {

	//STARTUP DEALY
	if (StartupDelay > 0)
		StartupDelay--;

	//UPDATE SYSTEM TIMER INFORMATION
	  ++tick;  //increment the tick value
	  for(i = 0; i < ntimers; ++i)
	  {
	      if(timers[i] > 0L)
	      {
	          --timers[i];
	      }
	  }


	  //HEARTBEAT
	#ifdef USE_LED_HEART_BEAT
	if (LedBlinkRate == LED_ALWAYS_OFF) {
		LED1_OFF;
	}
	else if (LedBlinkRate == LED_ALWAYS_ON) {
		LED1_ON;
	}
	else {
		LedBlinkTicker++;
		if (LedBlinkTicker >= LedBlinkRate) {
			LED1_TOGGLE;
			LedBlinkTicker = 0;
		}//if
	}//else
	#endif

}//



// void InitUSB_MSC() ***********************************************************
void InitUSB_MSC() {
	int n;

	MscDevInfo.idVendor = USB_VENDOR_ID;
	MscDevInfo.idProduct = USB_PROD_ID;
	MscDevInfo.bcdDevice = USB_DEVICE;
	MscDevInfo.StrDescPtr = (uint32_t) & USB_StringDescriptor[0];
	MscDevInfo.MSCInquiryStr = (uint32_t) & InquiryStr[0];
	MscDevInfo.BlockSize = 512;
	MscDevInfo.BlockCount = 4096;
	MscDevInfo.MemorySize = 2097152;
	MscDevInfo.MSC_Read = MSC_MemoryRead;
	MscDevInfo.MSC_Write = MSC_MemoryWrite;

	DeviceInfo.DevType = USB_DEVICE_CLASS_STORAGE;
	DeviceInfo.DevDetailPtr = (uint32_t) & MscDevInfo;

	/* Enable Timer32_1, IOCON, and USBREG blocks */
	LPC_SYSCON->SYSAHBCLKCTRL |= (EN_TIMER32_1 | EN_IOCON | EN_USBREG);

	(*rom)->pUSBD->init_clk_pins(); /* Use pll and pin init function in rom */

	/* insert a delay between clk init and usb init */
	for (n = 0; n < 75; n++) {
	}

	(*rom)->pUSBD->init(&DeviceInfo); /* USB Initialization */
	init_msdstate(); /* Initialize Storage state machine */
	(*rom)->pUSBD->connect(TRUE); /* USB Connect */

}

// USB_IRQHandler ***************************************************************
void USB_IRQHandler(void) {
	(*rom)->pUSBD->isr();
}




//EOF


