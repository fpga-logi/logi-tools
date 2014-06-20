/******************************************************************
* timers.c

example:
//setup the tick period and timer prescale values for timing

main{
TimerInit();
t1 = RegisterTimer();
TimerStart(t1, 1000L);   //starts t1, 1 second interval
if(!TimerIsOn(t1)) { 	//timer is - when finished
	//Do something    
}//if
}
use DelayMS(x) to see if tick is working
*******************************************************************/
#include "System.h"
#include "Timers.h"


uint16_t LED_BlinkSpeed = 0;
uint16_t LED_BlinkTick = 0;



//global variables
volatile int ntimers = 0; 	//counter for the n of counters registered
volatile uint32_t tick = 0;                     // tick counter
volatile uint32_t timers[MAX_TIMERS] = {0L};    // Timer array
static volatile uint16_t i;


//SYSTEM TIMERS DEFINED HERE
uint8_t timer_pb = 0;		//push button 0 timer ID
uint8_t flag_pb = 0;		//flag that indicates timer startesd.



/*****************************************************************
* init timer1
* Init timer hardware if needed here
******************************************************************/
void InitTimers(void)
{
	//SETUP ANY HARDWARE AS NEEDED

	//CHECKOUT THE TIMERS THAT WILL BE USED IN THE SYSTEM
	timer_pb = RegisterTimer();	//register the timer

	//START THE TIMERS IF THEY ARE FREE RUNNING
	TimerStart(timer_pb, TIME_PB0);	//restart the time

}




/**********************************************************************
* TimerTest
* Description: Basic test of timer functionality
* Notes:  Uses 1 timer slot - cannot be unregistered,  only use for
* 	testing and debugging.
***********************************************************************/
void TimerTest( void)
{
	uint8_t t1 = 0;

	t1 = RegisterTimer();	//register the timer
	TimerStart(t1, 1000L);   //starts t1, 1 second interval
	LED0_ON;					//turn on the LED
	while(TimerIsOn(t1));	//wait until timer has finished


	//SAME THING FOR LED OFF
	TimerStart(t1, 1000L);   //starts t1, 1 second interval
	LED0_OFF;
	while(TimerIsOn(t1));

}


//PUTTING THE ISR PROCESSING THE SYSTEM TICK IRQ
/***********************************************
* ISR for the timers
************************************************/
/*
//void __ISR(_CORE_TIMER_VECTOR, ipl2) CoreTimerHandler(void) {
void Timers_ISR(){

  ++tick;  //increment the tick value
  for(i = 0; i < ntimers; ++i)
  {
      if(timers[i] > 0L)
      {
          --timers[i];
      }
  }


	//add any discrete system period tasks here
	#ifdef USE_HEARTBEAT
    if (LED_BlinkSpeed == 0) {
        LED = 0;
        LED_BlinkTick = 0;
    } else if (LED_BlinkSpeed == 0xFFFF) {
        LED = 1;
        LED_BlinkTick = 0;
    } else {
        if (++LED_BlinkTick >= LED_BlinkSpeed) {
            LED_BlinkTick = 0;
            LED ^= 1;
			//TransmitString("testing output\r\n");
        }
    }
	#endif


    UpdateCoreTimer(CORE_TICK_RATE); // update the period of timer - REQUIRED
	mCTClearIntFlag();
}
*/



/****************************************************************
* getTick - returns the current tick value
* 	this only give access to the current tick value of
*	this c file
*****************************************************************/
uint32_t GetTick(void)
{ 
   return tick; 
} 
/*****************************************************************
* delay
* delay general purpose blocking delay
*****************************************************************/
//delay for tick milliseconds 
void DelayMS(uint16_t tickDel)
{ 
   unsigned long t; 
   
   t = tick + tickDel;
   while(tick < t);

} 


/******************************************************************
* registerTimer
*	registers a timer in array that will begin incrementing
*******************************************************************/
uint16_t RegisterTimer(void)
{ 
   int ret = -1; //error by default

   if(ntimers < MAX_TIMERS)	//check to make sure dont overflow max timer array 
   { 
       timers[ntimers] = 0; //sets value to 0 in the next array value available
       ret = ntimers; //returns the nth timer that it is initialized to
       ntimers++; 	//increment the # of timers value
   } 
   return ret; //return -1 if too many timers, or nth timer that was initialized
} 

/***********************************************
* timerStart
*	input: passes the index of timer that want to start
*		number of ticks to initialize to
*	return: none
*
************************************************/
void TimerStart(uint16_t timer, uint32_t ticks)
{ 
   if(timer >= 0 && timer < ntimers) //check to make sure not negative and smaller than max size
       timers[timer] = ticks; //initialize the timer tick value to that timer
} 
/**************************************************************
* timerStop
*	input: number of timer
	input the number of timer to stop, set the value to 0
***************************************************************/
void TimerStop(uint16_t timer)
{ 
   if(timer >= 0 && timer < ntimers) 
       timers[timer] = 0; 
} 

/****************************************************************
* timerIsOn
	input: number of timer
	returns: 1 if running, 0 if done
*****************************************************************/
uint16_t TimerIsOn(uint16_t timer)
{ 
   if(timer >= 0 && timer < ntimers && timers[timer] > 0) 
       return 1; 
   else 
       return 0; 
} 

/****************************************************************
TICK_PERIOD is set to give me a 1 millisecond period at 80 MHz. 

The timer functions are used as: 
*****************************************************************/


