#include "Chrono.h"
#include "MyTimer.h"
#include "Girouette.h"
#include "Accelerometre.h"
#include "Management.h"


static Time Chrono_Time; // rem : static rend la visibilité de la variable Chrono_Time limitée à ce fichier 
static TIM_TypeDef * Chrono_Timer=TIM1; // init par défaut au cas où l'utilisateur ne lance pas Chrono_Conf avant toute autre fct.
void Chrono_Task_10ms(void);
void Chrono_Background(void);

void Chrono_Background() {
		// MAIN PROGRAM. mettez ici tout vos fonctions pour le projet
		
	
}

void Chrono_Conf(TIM_TypeDef * Timer)
{
	Chrono_Time.Hund=0;
	Chrono_Time.Sec=0;
	Chrono_Time.Min=0;
	
	Chrono_Timer=Timer;
	MyTimer_Conf(Chrono_Timer,999, 719);
	MyTimer_IT_Conf(Chrono_Timer, Chrono_Task_10ms,3);
	MyTimer_IT_Enable(Chrono_Timer);
	
	
	// mettez ici toute vos fonctions de config de vos périph
	// ex : girouette_Conf(TIM3)...
	
}



void Chrono_Start(void)
{
	MyTimer_Start(Chrono_Timer);
}

void Chrono_Stop(void)
{
	MyTimer_Stop(Chrono_Timer);
}

void Chrono_Reset(void)
{
	Chrono_Time.Hund=0;
	Chrono_Time.Sec=0;
	Chrono_Time.Min=0;
}


Time * Chrono_Read(void)
{
	return &Chrono_Time;
}


void Chrono_Task_10ms(void)
{ 
	Chrono_Time.Hund++;
	if (Chrono_Time.Hund==100)
	{
		Chrono_Time.Sec++;
		Chrono_Time.Hund=0;
	}
	if (Chrono_Time.Sec==60)
	{
		Chrono_Time.Min++;
		Chrono_Time.Sec=0;
	}
	if (Chrono_Time.Min==60)
	{
		Chrono_Time.Hund=0;
	}
	
}


