#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_exti.h"



TIM_TypeDef* TIMx = TIM3;
#define _GIROUETTE_ARR	360*4

void girouetteConf(){
	
	/* On configure l'interface en mode codeur avec la bibliothèque LL*/
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); 
	LL_TIM_ENCODER_InitTypeDef My_LL_Tim_Encoder_Init_Struct;
	
	
	
	My_LL_Tim_Encoder_Init_Struct.EncoderMode	= LL_TIM_ENCODERMODE_X4_TI12;
	My_LL_Tim_Encoder_Init_Struct.IC1Polarity = LL_TIM_IC_POLARITY_RISING;
	My_LL_Tim_Encoder_Init_Struct.IC1Prescaler = LL_TIM_ICPSC_DIV1;
	My_LL_Tim_Encoder_Init_Struct.IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	My_LL_Tim_Encoder_Init_Struct.IC1Filter = LL_TIM_IC_FILTER_FDIV1;
	
	My_LL_Tim_Encoder_Init_Struct.IC2Polarity = LL_TIM_IC_POLARITY_RISING;
	My_LL_Tim_Encoder_Init_Struct.IC2Prescaler = LL_TIM_ICPSC_DIV1;
	My_LL_Tim_Encoder_Init_Struct.IC2ActiveInput = LL_TIM_ACTIVEINPUT_INDIRECTTI;
	My_LL_Tim_Encoder_Init_Struct.IC2Filter = LL_TIM_IC_FILTER_FDIV1;
	
	LL_TIM_ENCODER_Init(TIMx,&My_LL_Tim_Encoder_Init_Struct);
	
	/*Configuration du GPIO pour la lecture de l'entrée des pins*/
	
	/*Configuration de la première entrée*/
	
	//a6 a7
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_FLOATING);
	/*Configuration de la deuxième entrée*/
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_FLOATING);
	/*Index*/
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_FLOATING);

	LL_TIM_SetAutoReload (TIMx, 4*360);
	LL_TIM_EnableARRPreload (TIMx);

	
}


//Fonction handler pour garder en ligne de mire le comportement du channel index
void EXTI9_5_IRQnHandler(){
		
		/*LL_EXTI_ClearFlag_0_31 (LL_EXTI_LINE_5);*/
		
		LL_TIM_SetCounter(TIM3, 0);

	
		
}

float girouette_get_angle(){
	
	//TIMx est choisi en TIM3 voir variable global au début du code. 
	float angle;
	int cnt = LL_TIM_GetCounter (TIMx);
	angle = cnt/(4.);
	return angle;

		
	}
