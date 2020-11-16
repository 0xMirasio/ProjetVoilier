#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_exti.h"
#include "servomoteurs.h"
#include "MyTimer.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "Girouette.h"

#define _SERVO_MOTOR_MAX_ANGLE 90.0 //ou 60 ?
#define _SERVO_ARR 19999 //5624
#define _SERVO_PSC 71 //255

#define A_ALPHA_TO_TETHA (2.0/3.0) //Coefficient directeur de la partie linéaire de la fonction transformant l'allure en angle de voile
#define B_ALPHA_TO_TETHA (-30.0) //Ordonnée a l'origine de cette même fonction
#define CRR_PWM_90 (19900) //Valeur du registre commandant la largeur de la PWM pour les voiles lachées
#define CRR_PWM_0 (100) //Valeur du registre commandant la largeur de la PWM pour les voiles bordées au maximum
#define A_TETHA_TO_CRR_PWM ((CRR_PWM_90 - CRR_PWM_0)/90.0) //Coefficient directeur de la relation entre l'angle de voile et la PWM
#define B_TETHA_TO_CRR_PWM (CRR_PWM_0) //Ordonnée a l'origine


static TIM_TypeDef *TimerServo;


void servo_start(TIM_TypeDef *timer)
{
	TimerServo = timer;	
	servoConf();
	servo_setAngle(0);
}

void servoConf(void){
	
	//Configuration des GPIO
	
	LL_GPIO_InitTypeDef My_LL_Gpio_Init_Struct;
	
	My_LL_Gpio_Init_Struct.Pin = LL_GPIO_PIN_8;
	My_LL_Gpio_Init_Struct.Mode = LL_GPIO_MODE_ALTERNATE;
	My_LL_Gpio_Init_Struct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	My_LL_Gpio_Init_Struct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	My_LL_Gpio_Init_Struct.Pull = LL_GPIO_PULL_DOWN;
	
		if (TimerServo == TIM1) {
		/* Enable clock */
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1); 
		
		LL_GPIO_Init(GPIOA, &My_LL_Gpio_Init_Struct);
			
	} else if (TimerServo == TIM4) {//Cf datasheet du STM32
		/* Enable clock */
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4); 
		
		LL_GPIO_Init(GPIOB, &My_LL_Gpio_Init_Struct);
	}
}

void servo_setAngle(int angleGirouette)
{
	/* Servo moteur contained in [0°;60°] */
	/* CRR = (degree * ARR) / 60  */
	
	float angleVoile;
	int crr;
	
	
	if (angleGirouette < 45 || angleGirouette > 315) {
		angleVoile = _SERVO_MOTOR_MAX_ANGLE;
	}
	
	else if (angleGirouette > 45 && angleGirouette <= 315) {
		angleVoile = A_ALPHA_TO_TETHA * (float)abs(angleGirouette) + B_ALPHA_TO_TETHA;
	}
	 crr = TimerServo->CCR1;
	 crr = (int)(A_TETHA_TO_CRR_PWM * angleVoile + B_TETHA_TO_CRR_PWM) ;
	
	Timer_PWM_high_level_counter(TimerServo, crr);
}


