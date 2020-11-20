#include "Chrono.h"
#include "MyTimer.h"
#include "Girouette.h"
#include "Accelerometre.h"
#include "servomoteurs.h"
#include "Management.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_rcc.h"
#include "plateau.h"

#define separator 0x3a // 0x3a = :
#define retour 0x0D // 0x0D = \n 
#define usart USART2 
#define seuil ((uint32_t)1500)


static Time Chrono_Time; // rem : static rend la visibilité de la variable Chrono_Time limitée à ce fichier 
static TIM_TypeDef * Chrono_Timer=TIM3; // Timer chrono par défault
static int cpt=0;
static float angle=  0;
static float vitesse = 0;
static uint16_t voltage;
void Chrono_Task_10ms(void);
void Chrono_Background(void);

void Chrono_Background() {

		//surveillance angle
		
		angle = girouette_get_angle(); // on recupère l'angle 
		servo_setAngle(angle); 
		
		// surveillance VITESSE
		vitesse = get_vitesse_sens();
		bougerPlateau();
		
		// Mouvement des voiles
		bougerVoile(getAngle());
		// SURVEILLANCE BATTERIE
		voltage = getAlimentationState();
		if (voltage < seuil) {
				Usart_sendBatInfo(USART2, voltage, &Chrono_Time);
		}
		else {
				usart_sendClearMessage(USART2, voltage, &Chrono_Time);
		}
				
		
}

void Chrono_Conf(TIM_TypeDef * Timer)
{
	Chrono_Time.Hund=0;
	Chrono_Time.Sec=0;
	Chrono_Time.Min=0;
	Chrono_Time.Hour=0;
	
	Usart_conf(USART2);
	Adc_Conf(ADC2); // on config l'adc2 a utiliser pour l'usart
	
		//init RF (avec PWM input)
	gpio_RF_init();
	timer_RF_init(); // configure TIM4 => module RF
	
	//init mcc + PWM
	gpio_mcc_init();
	timer_pwm_mcc_init(); // configure TIM2 => moteur CC
	
	Adc_Conf_ACC(ADC1); // adc1 utilisé pour la recuperation de l'angle
	timer_pwm_init(); // TIM1 pour le servomoteur
	gpio_servom_init(); // pin gpio pour le servo
	
	
	girouetteConf(); // configuration girouette	TIM3
	MyTimer_Start(TIM3);
	MyTimer_IT_Enable(TIM3);
	//Timer_PWM_output_conf(TIM1, 19999, 71); // conf pwn tim1 (identique que timer_pwn_init)
	servo_start(TIM1); 
	
	
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


