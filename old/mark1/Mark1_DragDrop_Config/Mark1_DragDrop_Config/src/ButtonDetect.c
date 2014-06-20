#include "System.h"
#include "DataTypes.h"
#include "ButtonDetect.h"

//#include "System.h"

#define BTN_DEBOUNCE_TIME 	3	//incrments of 10ms


#define  DEBOUNCE_STATE_WAIT_FOR_PRESS	0
#define  DEBOUNCE_WAIT_FOR_STABLE_ON	1
#define  DEBOUNCE_WAIT_FOR_RELEASE		2
#define  DEBOUNCE_WAIT_FOR_STABLE_OFF	3


volatile ButtonDetect Btn0;	//push button 0 debounce structure

//extern  ButtonDetect Button_SW2;
//Always used these function to check button states.  THe preopely check the structure
//and reset flags.


/**********************************************************************************************
* Process all the pushbutton in one call.
***********************************************************************************************/
void ButtonProcessAll( void){

	ButtonProcess( &Btn0, !PB_STATE);	//1 = active btn push ; 0 = idle

}


//SETUP ALL THE BUTTON DETECT STRUCTURES
void InitButtonDetectAll( void)
{

	InitButtonDetect(&Btn0, BTN_DEBOUNCE_TIME);

}

/**********************************************************************************************
* Process all the pushbutton in one call.
***********************************************************************************************/
void InitButtonDetect(ButtonDetect *BD,BYTE DebounceTime)
{
	BD->ButtonPressed = FALSE;
	BD->ButtonReleased = FALSE;
	BD->DebounceTime = DebounceTime;
	BD->DetectState = DEBOUNCE_STATE_WAIT_FOR_PRESS;
	BD->Ticker = 0;
}




//Returns True is there was a press event, false if not.   
BOOL ButtonPressed(ButtonDetect *BD)
{
	if(BD->ButtonPressed)
		{
			BD->ButtonPressed=FALSE;
			return TRUE;
		}
	else
		{
			return FALSE;
		}
}

//Returns zero if a released event has not occured yet.  If greater than zero a release event
//has occurred and the value is the button hold time in 10mSec Increments
BOOL ButtonReleased(ButtonDetect *BD)
{
	WORD RetVal;

	if(BD->ButtonReleased>0 && (BD->DetectState==DEBOUNCE_STATE_WAIT_FOR_PRESS))
		{
			RetVal = BD->ButtonReleased;
			BD->ButtonReleased = 0;			
		}
	else
		{
			RetVal = 0;
		}

	return RetVal;
}



//This Process must be called every 10mSec for each button detect
//The input is high active logic. i.e.  Input should be !!!THE LOGIC OF THE BUTON PRESS - YES
//a value greater than 0 if true, 0 if false;
void ButtonProcess(ButtonDetect * BD, BOOL Input)
{
	switch(BD->DetectState)
	{
		default:
		case  DEBOUNCE_STATE_WAIT_FOR_PRESS:
			if(Input>0 )
			{
				BD->DetectState=DEBOUNCE_WAIT_FOR_STABLE_ON;
				BD->Ticker =0 ;
			}
		break;
	
		case  DEBOUNCE_WAIT_FOR_STABLE_ON:	
		  if(Input>0)
		  {	
		     if(BD->Ticker++>=BD->DebounceTime)
				{
					BD->Ticker=0;
					BD->DetectState=DEBOUNCE_WAIT_FOR_RELEASE;
					BD->ButtonPressed = TRUE;
				}
		  }
  		 else
		 {
			BD->DetectState=DEBOUNCE_STATE_WAIT_FOR_PRESS;
		}
		break;

		case  DEBOUNCE_WAIT_FOR_RELEASE:	
		 if(Input>0)
		  {	
			if(BD->Ticker<0xFFFF)
				BD->Ticker++;
		  }
		else
			{
				BD->DetectState=DEBOUNCE_WAIT_FOR_STABLE_OFF;
				BD->ButtonReleased = BD->Ticker;
				BD->Ticker = 0;
			}
			
	
		break;

		case  DEBOUNCE_WAIT_FOR_STABLE_OFF:	
			if(Input == 0)
			{
				BD->Ticker++;
				if(BD->Ticker >= BD->DebounceTime)
				{
					BD->DetectState=DEBOUNCE_STATE_WAIT_FOR_PRESS;	
				}
			}
		break;
	}
}


//debug code:
/*
if(ButtonPressed(&Btn0)){
	LED0_ON;
}
if(ButtonReleased(&Btn0)){
	LED0_OFF;
}
*/




//EOF
