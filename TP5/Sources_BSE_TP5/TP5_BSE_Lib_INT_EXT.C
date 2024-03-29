/******************************************************************************
; 	FILE NAME  	: TP5_BSE_Lib_Timers_UART.C
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configurations des Timers et des UART
;
; 	REVISION 1.0
;   
*/
//******************************************************************************

#include "C8051F020.h"
#include "c8051F020_SFR16.h"
#include "TP5_BSE_Lib_INT_EXT.h"


//*****************************************************************************	 
//******************************************************************************


//******************************************************************************
void Config_INT7(void)
{
	P3IF &= ~(1<<7); // IE7 mis � 0 pending flag de INT7 effac�
	P3IF &= ~(1<<3); // IE7CF mis � 0 - sensibilit� int7 front descendant	
	
	EIP2 &= ~(1<<5);  // PX7 mis � 0 - INT7 priorit� basse
	EIE2 |= (1<<5);  // EX7 mis � 1 - INT7 autoris�e
}

//*****************************************************************************	 
//******************************************************************************
void Config_INT6(void)
{
	P3IF &= ~(1<<7); // IE6 mis � 0 pending flag de INT6 effac�
	P3IF &= ~(1<<2); // IE6CF mis � 0 - sensibilit� int6 front descendant	
	
	EIP2 &= ~(1<<4);  // PX6 mis � 0 - INT7 priorit� basse
	EIE2 |= (1<<4);  // EX6 mis � 1 - INT7 autoris�e
}