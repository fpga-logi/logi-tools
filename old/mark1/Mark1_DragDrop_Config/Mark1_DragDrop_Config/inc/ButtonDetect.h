#include "DataTypes.h"

#ifndef _BUTTON_DETECT_H
#define _BUTTON_DETECT_H

typedef struct 
{
	BOOL ButtonPressed;
	WORD ButtonReleased;	
	BYTE DebounceTime;
	BYTE DetectState;
	WORD Ticker;
} ButtonDetect;

//PUSH BUTTON STRUCTURE USED FOR DEBOUNCING
//extern volatile ButtonDetect pBtn0;
extern volatile ButtonDetect Btn0;	//push button 0 debounce structure



void InitButtonDetectAll( void);
void ButtonProcessAll( void);
void InitButtonDetect(ButtonDetect *BD,BYTE DebounceTime);
//This Process must be called every 10mSec for each button detect
//The input is high active logic. i.e.  Input should be
//a value greater than if true, 0 if false;
void ButtonProcess(ButtonDetect * BD,BOOL Input);
//Always used these function to check button states.  THe preopely check the structure
//and reset flags.
//Returns True is there was a press event, false if not
BOOL ButtonPressed(ButtonDetect *BD);
//Returns zero if a released event has not occured yet.  If greater than zero a release event
//has occurred and the value is the button hold time in 10mSec Increments
BOOL ButtonReleased(ButtonDetect *BD);

#endif
