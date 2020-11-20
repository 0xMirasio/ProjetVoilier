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

/* Fonction récupérant l'angle de la girouette et envoyant l'angle à atteindre pour les servomoteurs */
void servo_setAngle(int angleGirouette)
{
	/* 	angle girouette 0-360°
	angle servo : 0-90°
	valeur_servo_0° = 450
	valeur_servo_max = 1200
	soit un coef lineaire a = 38.98, offset = -2408*/

	int angleGirouetteV1=angleGirouette%360; //On place un modulo 360 pour prendre en compte le passage à zéro de la girouette.
	
	//min = 450 , max = 1200
	TimerServo->CCR1 = angleGirouetteV1*38.98 - 2408; //équation de droite affine en fonction de l'angle de la girouette et du rapport cyclique représenté 
	//par CCR1
}

