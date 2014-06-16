/***************************************************************
* timers.h
****************************************************************/
#ifndef _TIMERS_H
#define _TIMERS_H

//DEFINES
#define MAX_TIMERS  10


//DEFAULT VALUES USED BY TIMERS
#define TIME_PB0 10		//count in ms
#define TIME_PB1 10


//TIMER ID'S USED IN SYSTEM
extern uint8_t timer_pb;		//push button 0 timer ID
extern uint8_t flag_pb;		//flag for push button 0


//VARIABLES
extern volatile uint32_t tick;              // tick counter
extern volatile uint32_t timers[MAX_TIMERS];    // Timer a
extern volatile int ntimers;


//FUNCTION PROTOTYPES
void InitTimers(void);
uint32_t GetTick(void);
void DelayMS(uint16_t tickDel);
uint16_t RegisterTimer(void);
void TimerStart(uint16_t timer, uint32_t ticks);
void TimerStop(uint16_t timer) ;
uint16_t TimerIsOn(uint16_t timer) ;


#endif


//EOF
