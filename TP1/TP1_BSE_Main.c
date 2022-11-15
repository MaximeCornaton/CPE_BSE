//-----------------------------------------------------------------------------
// TP1_BSE.c
//-----------------------------------------------------------------------------
// AUTH: LAJUGIE Rodolphe, CORNATON Maxime
// DATE: 15/11/2022
//
// Target: C8051F02x
// Tool chain: KEIL Microvision5
//
//-----------------------------------------------------------------------------
// Déclarations Registres et Bits de l'espace SFR
#include "intrins.h"
#include<c8051F020.h>
#include<c8051F020_SFR16.h>
#include<TP1_BSE_Lib_Config_Globale.h>
#include<TP1_BSE_Lib_Divers.h>

#define LED_ON 1
#define LED_OFF 0

#define BP_PRESSED 1
#define BP_NOT_PRESSED 0

#define CLIGNOTEMENT 1

#define START 1
#define END 0

sbit LED = P1^6;  // LED
sbit BP = P3^7; //BP

sbit VISU_INT7_START = P2^0;
//sbit VISU_INT7_END = 0x86;
//sbit VISU_INT7_WIDTH = 0x87;

bit ETAT_LED = 1;
bit ACK_BP = 1;

//------------------------------------------------------------------------------------
// Function Prototypes

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------



//Le cod.e de l’interruption INT7
//****************************************************************
void ISR_INT7() interrupt 19
{		VISU_INT7_START = START;
		VISU_INT7_START = END;
	
		P6 |= (1<<1); //VISU_INT7_WIDTH
	
		ETAT_LED = !ETAT_LED;
		P3IF &= ~(1<<7); 
	
		P6 &= ~(1<<1); //VISU_INT7_WIDTH
	
		P6 |= (1<<0); //VISU_INT7_END
		P6 &= ~(1<<0); //VISU_INT7_END
}


void Config_INT7_Ext(void){ //p109/110
		P3IF &= ~(1<<7); //Pending Flag =0
		P3IF |= (0<<3); //Declenchement sur front descendant
		EIE2 |= (1<<5); //Autorise l'interruption sur INT7
		EIP2 |= (1<<5); //Priorité
}


//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------


void main (void) {
		Init_Device();
	
		//Accès au port en mode GPIO sortie du port
		P3MDOUT &= ~(1<<7);
		BP = 1;
	
		Config_INT7_Ext();
		EA = 1;
		
		P2MDOUT |= (1<<0);
		P3MDOUT &= ~(1<<7);
		P74OUT |= (1<<4);
		
		while(1)
		{
				if(ETAT_LED == CLIGNOTEMENT){			
						LED = LED_ON;
						Software_Delay(2); // Allumage 20ms
						LED = LED_OFF;
						Software_Delay(10); // Extinction 100ms
				}
		}
        
}
//*****************************************************************************    
