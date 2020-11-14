#include "Chrono.h"
#include "MyTimer.h"
#include "Girouette.h"
#include "Accelerometre.h"
#include "Management.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_rcc.h"

#define separator 0x3a // 0x3a = :
#define retour 0x0D // 0x0D = \n 
#define usart USART2 
#define seuil ((uint32_t)1500)


static Time Chrono_Time; // rem : static rend la visibilit� de la variable Chrono_Time limit�e � ce fichier 
static TIM_TypeDef * Chrono_Timer=TIM1; // init par d�faut au cas o� l'utilisateur ne lance pas Chrono_Conf avant toute autre fct.
static int cpt=0;
static uint16_t voltage;
void Chrono_Task_10ms(void);
void Chrono_Background(void);

void Chrono_Background() {

		// SURVEILLANCE BATTERIE
		voltage = getAlimentationState();
		if (voltage < seuil) {
				Usart_sendBatInfo(USART2, voltage, &Chrono_Time);
		}
		else {
				usart_sendClearMessage(USART2, voltage, &Chrono_Time);
		}
				
		// SURVEILLANCE DONNEES BATEAU
		//TODO
}

void Chrono_Conf(TIM_TypeDef * Timer)
{
	Chrono_Time.Hund=0;
	Chrono_Time.Sec=0;
	Chrono_Time.Min=0;
	Chrono_Time.Hour=0;
	
	Chrono_Timer=Timer;
	MyTimer_Conf(Chrono_Timer,999, 719);
	MyTimer_IT_Conf(Chrono_Timer, Chrono_Task_10ms,3);
	MyTimer_IT_Enable(Chrono_Timer);
	
	Usart_conf(USART2);
	Adc_Conf(ADC1);
	
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
	Chrono_Time.Hour=0;
}


Time * Chrono_Read(void)
{
	return &Chrono_Time;
}


void Chrono_Task_10ms(void)
{ 
	cpt++;
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
		Chrono_Time.Hour++;
		Chrono_Time.Min = 0;
	}
	
}


