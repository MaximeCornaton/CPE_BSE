//-----------------------------------------------------------------------------
// TP4_BSE_Main.c
// AUTH: FJ, LAJUGIE Rodolphe, CORNATON Maxime
// DATE: 5/12/21
//
// Target: C8051F02x
// Tool chain: KEIL Microvision5
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Cette application ex�cute les t�ches suivantes:
// SYSCLK = Quartz externe = 22.1184 MHz (Visu sur P1.0)
// Clignotement de la LED P1.6
// Commande du clignotement de la LED avec le bouton poussoir P3.7 (INT7) et 
// via le signal DECL_EXT sur P3.6 (INT6)

// Mise en oeuvre de la base de temps Timer2 de 10ms et interruption Timer2

// Mise en oeuvre d'un comptage d'�v�nements SIG_IN (sur T4)
// Le comptage d'�v�nement est assur� par le Timer4 et par une interruption Timer4
// Comptage Programmable selon les facteurs 10-100-1000-10000
// La programmation du comptage est assur�e au travers des niveaux sur P2.5 et P2.6
// Le signal SIG_OUT change d'�tat � chaque fois que l'on a compt� 10...10000 �v�nements


// Calcul de la fr�quence de SIG_IN et stockage dans la variable Frequence
// La mesure de fr�quence est assur�e par une mesure du nombre d'�v�nements sur une seconde
// Mesure r�alis�e dans l'interruption Timer2

// Visu Flag INT7 sur P2.4
// Visu Flag INT6 sur P6.4
// Visu Flag INT Timer2 sur P3.5
// Visu Flag INT Timer4 sur P3.4

// Visu SIG_OUT sur P3.3

//*****************************************************************************
// Fichiers d'ent�te
#include "intrins.h"
#include<c8051F020.h>
#include<c8051F020_SFR16.h>
#include<TP4_BSE_Lib_Config_Globale.h>
#include<TP4_BSE_Lib_Divers.h>
#include<TP4_BSE_Main.h>

#include<stdio.h>
void Cfg_CLK_UART0(void);
void Cfg_UART0(void);
char putchar(char);
//-----------------------------------------------------------------------------
// D�claration des MACROS

#define SYSCLK 22118400L
#define LED_ON 1

#define LED_OFF 0
#define LED_BLINK 0
#define BP_ON 0
#define BP_OFF 1
#define TO_BE_PROCESSED 1
#define PROCESSED 0
#define SET_VISU_INT6 P6 |= (1<<4)
#define RESET_VISU_INT6 P6 &= ~(1<<4)
//-----------------------------------------------------------------------------
// D�clarations Registres et Bits de l'espace SFR
sbit LED = P1^6;  // LED
sbit BP =P3^7;
sbit VISU_INT7 = P2^4;
sbit VISU_INT_TIMER2 = P3^5;
sbit VISU_INT_TIMER4 = P3^4;
sbit SIG_OUT = P3^3;
//-----------------------------------------------------------------------------
// Variables globales

bit Event = PROCESSED;  // D�tection des �v�nements pour changer le clignotement de la LED
unsigned int Event_to_Count = 100 ; //Valeur du comptage d'�v�nements 
long frequence = 0;  // Fr�quence calcul�e de SIG_IN
unsigned int CP_Overflow_Timer4; // Compteur d'overflows du Timer4
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) {
	


 	   // Configurations globales
	      Init_Device();
	     
	   // Configurations  sp�cifiques  
	      Config_INT7(); // Configuration de INT7
	      Config_INT6(); // Configuration de INT6
	      Config_Timer2_TimeBase();
        Config_Timer4_Event_Counter();
				Cfg_CLK_UART0();
				Cfg_UART0();
	   // Fin des configurations
	      
	      EA = 1;  // Validation globale des interruptions
	
// Boucle infinie	
         while(1){
				 
					printf("Mesure frequence : %ld Hz\n", frequence);

					
				 Software_Delay(1000); //1s

				 }
					
			}
//******************************************************************************
void Config_INT7(void)
{
	P3IF &= ~(1<<7); // IE7 mis � 0 pending flag de INT7 effac�
	P3IF &= ~(1<<3); // IE7CF mis � 0 - sensibilit� int7 front descendant	
	
	EIP2 &= ~(1<<5);  // PX7 mis � 0 - INT7 priorit� basse
	EIE2 |= (1<<5);  // EX7 mis � 1 - INT7 autoris�e
}
//******************************************************************************
void ISR_INT7 (void) interrupt 19
{
	VISU_INT7 = 1;
	P3IF &= ~(1<<7); // IE3 mis � 0 - remise � z�ro du pending flag de INT7 effac�
	Event = TO_BE_PROCESSED;
	VISU_INT7 = 0;
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

//******************************************************************************
void ISR_INT6 (void) interrupt 18
{
	SET_VISU_INT6;
	P3IF &= ~(1<<6); // IE6 mis � 0 - remise � z�ro du pending flag de INT6 effac�
	P3IF ^= (1<<2);   // Action sur IE6CF - Commutation Front montant / Front Descendant
	Event = TO_BE_PROCESSED;
	RESET_VISU_INT6;
}	

//*****************************************************************************	 
//******************************************************************************
void Config_Timer2_TimeBase(void)
{
	CKCON &= ~(1<<5);         // T2M: Timer 2 Clock Select
                         // CLK = sysclk/12TR2 = 0;  //Stop Timer
	TF2 = 0;  // RAZ TF2
	EXF2 = 0;  // RAZ EXF2
  RCLK0 = 0;         
  TCLK0 = 0;
  CPRL2 = 0;  // Mode AutoReload	
	EXEN2 = 0;   // Timer2 external Enable Disabled 
  CT2 = 0;    // Mode Timer

	RCAP2 = -((SYSCLK/12)/100);
  T2 = RCAP2;
  TR2 = 1;                           // Timer2 d�marr�
  PT2 = 1;							  // Priorit� Timer2 Haute

   ET2 = 1;							  // INT Timer2 autoris�e
}

//******************************************************************************
void ISR_Timer2 (void) interrupt 5
{
	static char CP_Cligno;
	static char CP_Seconde = 0;
	static bit STATE_LED = LED_BLINK;
	char Read_Cfg_Event;
	static unsigned int CP_Timer4 = 0;
	static unsigned int OLD_CP_Timer4 = 0;

	VISU_INT_TIMER2 = 1;
	
	
	if (TF2 == 1)
	{
		TF2 = 0;
	
	// Gestion de la mesure de frequence - Synth�se
  // Le calcul de la fr�quence est fait toutes les secondes		
	CP_Seconde++;
	if (CP_Seconde >= 100) 
	{
		CP_Seconde = 0;
		T4CON &= ~(1<<2);   // TR4 = 0 -- Timer4 stopp�
		CP_Timer4 = T4;     // Lecture Compteur Timer4
		T4CON |= (1<<2);   // TR4 = 1 -- Timer4 red�marr�
		// Fr�quence = nbre total d'�v�nements par seconde
		frequence = ((CP_Overflow_Timer4) * (long)Event_to_Count) +
		            (int)(CP_Timer4 - OLD_CP_Timer4);
		            
		OLD_CP_Timer4 = CP_Timer4;
		CP_Overflow_Timer4 = 0;
		
	}	
	// Gestion des �v�nements INT6 et INT7
	// pour g�rer les modes de clignotement de la LED
		if (Event == TO_BE_PROCESSED)
						 {
							 Event = PROCESSED;
							 STATE_LED =  !STATE_LED;	
						 }
	// Gestion du mode de clignotement de la LED					 
    CP_Cligno++;
	  if (CP_Cligno > 11) CP_Cligno = 0;
    if (STATE_LED == LED_BLINK)
						{
							if (CP_Cligno < 2) LED = LED_ON;
							else LED = LED_OFF;
						}
						else LED = LED_OFF;		
						
	// Etape5 - Lecture consigne du nbre d'�v�nements � compter	pour g�n�rer SIG_OUT			
    Read_Cfg_Event = (P2 & ((1<<5)|(1<<6)))>>5; // Lecture consigne sur P2.5 et P2.6
    switch (Read_Cfg_Event)
		{
    	case 0:
				Event_to_Count = 10; 
    		break;
    	case 1:
				Event_to_Count = 100; 
    		break;
			case 2:
				Event_to_Count = 1000; 
    		break;
			case 3:
				Event_to_Count = 10000; 
    		break;
    	default:
				Event_to_Count = 100; 
    		break;
    }
	}
	// S�curit�: si EXF2 est � 1 - RAZ de EXF2 	
	if (EXF2 == 1)
	{
		EXF2 = 0;
	}
	
	VISU_INT_TIMER2 = 0;
}	


//******************************************************************************
void Config_Timer4_Event_Counter(void) // Pour �tape 4
{
	// Timer 4 configur� en compteur d'�v�nements SIG_IN
	// SIG_IN c�bl� sur l'entr�e T4 du Timer4
	
	// Timer4 en mode autorechargement
	// Mode Counter
	// CLKTimer = SIG_IN
	// Valeur rechargement= (65536 - Nbre_Event_to_count)
	// L'overflow, et donc l'interruption Timer4 signale le comptage de 100 �v�nements
	T4CON = 0x02;   // Flags TF4 et EXF4 effac�s
	                // RCLK1 et TCLK1 � z�ro
	                // Transitions sur T4EX ignor�es
	                // Timer stopp�
	                // Timer en mode counter
	                // Mode auto-reload
  RCAP4 = 65536-Event_to_Count;
  T4 = RCAP4;
  T4CON |= (1<<2);      // TR4 = 1 -- Timer2 d�marr�
  EIP2 |= (1<<2);      //PT4 = 0 -- Priorit� Timer4 haute
  EIE2 |= (1<<2);      //ET4 = 1 --  INT Timer4 autoris�e
}

//******************************************************************************
void ISR_Timer4 (void) interrupt 16  // Etape4
{
	
	
	VISU_INT_TIMER4 = 1; 
	
	if ((T4CON & (1<<7)) != 0)   // Test TF4 = 1
	{
		T4CON &= ~(1<<7);    // RAZ TF4
		SIG_OUT = !SIG_OUT;	  // G�n�ration SIG_OUT
		// Prise en compte d'un comptage d'�v�nements configurable
		// La lecture de la config 10-100-1000-10000 est faite dans ISR_Timer2
    RCAP4 = 65536 - Event_to_Count;	//Reload compteur d'�v�nements
    // Comptage des overflows (pour compter au del� de 65536)		
		CP_Overflow_Timer4++;
	}
	
	// S�curit� si T4EX est � 1
	if ((T4CON & (1<<6)) != 0)   // Au cas o�: Test EXF4 = 1
	{	T4CON &= ~(1<<6); } // RAZ EXF4			
	
	VISU_INT_TIMER4 = 0;
}	


//****************************************************

void Cfg_CLK_UART0(void)
{	
	TCON &= ~(1<<6); //Timer1 Stopp� (pr�caution)
	TCON &= ~(1<<7);//Flag Timer1 effac�
	CKCON |= (1<<4); //Config CKCON - | T1M: Timer1 ClockSelect � CLK Timer = Sysclk
	
	TMOD |=(1<<5);//Config TMOD - Timer1 configur� en timer 8 bit avec auto-reload
	TMOD &= ~(1<<4);//Config TMOD - Timer1 configur� en timer 8 bit avec auto-reload
	
	TH1 = 0xBB;//Programmation du registre de rechargement TH1
	//Initialisation du registre Timer TL1 � N�cessaire? /Non car ar defaut � 0.
	IE &= ~(1<<3); //D�validation de l�interruption Timer1 � Action sur registre IE
	TCON |= (1<<6); //Timer1 d�marr� - � Action sur TR1 de TCON
}

void Cfg_UART0(void)
{	
	SCON0 |= (1<<4);
	SCON0 &= ~(1<<0);
	TI0 = 1;
	SCON0 |= (1<<5);
	SCON0 &= ~(1<<7);
	SCON0 |= (1<<6);

	T2CON &= ~(1<<4);
	T2CON &= ~(1<<5);
	
	PCON |= (1<<7);
	PCON &= ~(1<<6);
}

//****************************************************

char putchar(char c)
{	
	while(!TI0);
	
	TI0 = 0;
	SBUF0 = c;
	return c;
}