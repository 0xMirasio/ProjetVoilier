

#include "MyTimer.h"
#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges
#include "stm32f1xx_ll_tim.h" 

void (*Ptr_ItFct_TIM1)(void); 
void (*Ptr_ItFct_TIM2)(void); 
void (*Ptr_ItFct_TIM3)(void); 
void (*Ptr_ItFct_TIM4)(void); 


void MyTimer_Conf(TIM_TypeDef * Timer,int Arr, int Psc)
{
	LL_TIM_InitTypeDef My_LL_Tim_Init_Struct;
	
	// Validation horloge locale
	if (Timer==TIM1) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	else if (Timer==TIM2) LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	else if (Timer==TIM3) LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	else  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	
	// chargement structure Arr, Psc, Up Count
	My_LL_Tim_Init_Struct.Autoreload=Arr;
	My_LL_Tim_Init_Struct.Prescaler=Psc;
	My_LL_Tim_Init_Struct.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	My_LL_Tim_Init_Struct.CounterMode=LL_TIM_COUNTERMODE_UP;
	My_LL_Tim_Init_Struct.RepetitionCounter=0;
	
	LL_TIM_Init(Timer,&My_LL_Tim_Init_Struct);
	

	// Blocage IT
	LL_TIM_DisableIT_UPDATE(Timer);
	
	
	// Blocage Timer
	LL_TIM_DisableCounter(Timer);
	

		
}

void MyTimer_Start(TIM_TypeDef * Timer)
{
		LL_TIM_EnableCounter(Timer);
}


void MyTimer_Stop(TIM_TypeDef * Timer)
{
	LL_TIM_DisableCounter(Timer);
}


void MyTimer_IT_Conf(TIM_TypeDef * Timer, void (*IT_function) (void),int Prio)
{
	// affectation de la fonction
	if (Timer==TIM1) Ptr_ItFct_TIM1=IT_function;
	else if (Timer==TIM2)	Ptr_ItFct_TIM2=IT_function;
	else if (Timer==TIM3)	Ptr_ItFct_TIM3=IT_function;
	else  Ptr_ItFct_TIM4=IT_function;

	
	// Blocage IT (il faudra la débloquer voir fct suivante)
	LL_TIM_DisableIT_UPDATE(Timer);
	
	// validation du canal NVIC
	IRQn_Type TIM_irq;
	
	if (Timer==TIM1) TIM_irq=TIM1_UP_IRQn;
	else if (Timer==TIM2)	TIM_irq=TIM2_IRQn;
	else if (Timer==TIM3)	TIM_irq=TIM3_IRQn;
	else 	TIM_irq=TIM4_IRQn;
	
	NVIC_SetPriority(TIM_irq, Prio);
	NVIC_EnableIRQ(TIM_irq);

	
}

void MyTimer_IT_Enable(TIM_TypeDef * Timer)
{
		LL_TIM_EnableIT_UPDATE(Timer);
}


void MyTimer_IT_Disable(TIM_TypeDef * Timer)
{
			LL_TIM_DisableIT_UPDATE(Timer); 
}	


void TIM1_UP_IRQHandler(void)
{
	// rabaisser le flag d'IT
	LL_TIM_ClearFlag_UPDATE(TIM1);
	(*Ptr_ItFct_TIM1)();
}	

void TIM2_IRQHandler(void)
{
	// rabaisser le flag d'IT
	LL_TIM_ClearFlag_UPDATE(TIM2);
	(*Ptr_ItFct_TIM2)();
}	

void TIM3_IRQHandler(void)
{
	// rabaisser le flag d'IT
	LL_TIM_ClearFlag_UPDATE(TIM3);
	(*Ptr_ItFct_TIM3)();
}	

void TIM4_IRQHandler(void)
{
	// rabaisser le flag d'IT
	LL_TIM_ClearFlag_UPDATE(TIM4);
	(*Ptr_ItFct_TIM4)();
}	

/*
void Timer_PWM_output_conf(TIM_TypeDef *timer, int Arr, int Psc)
{
	
	MyTimer_Conf(timer ,Arr, Psc);
	
	if (timer == TIM4) {

			timer->CCMR2 |= 0x6 << 4;
			timer->CCMR2 &= ~(0x1 << 4);
			
			timer->CCMR2 |= 0x1 << 3;
		
			timer->CR1 |= 0x1<<7; 

			timer->EGR |= 0x1;

			timer->CCER |= 0x1 << 8;
			
			timer->CCER |= 0x1 << 9;
			
			timer->CCR3 = 0;

		
	} else if (timer == TIM1) {

			timer->CCMR1 |= 0x6 << 4;
			timer->CCMR1 &= ~(0x1 << 4);
			
			timer->CCMR1 |= 0x1 << 3;

			timer->EGR |= 0x1;

			timer->CCER |= 0x1;
			timer->CCER &= ~(0x1 << 1);
			timer->CCER |= 0x1 << 2;
		
			//MOE, OSSI and OSSR bits (TIMx_CCER and TIMx_BDTR registers).
			//Refer to the TIMx_CCER register description for more details.
			//timer->BDTR |= 0x1 << 10; //OSSI
			timer->BDTR |= 0x1 << 11; //OSSR 
			timer->BDTR |= 0x1 << 15; // MOE  
			
			timer->CCR1 = 0x7f;
	}
	
	// Enable edge-aligned PWM mode
	timer->CR1 &= ~(0x1 << 4);
	timer->CR1 &= ~(0x3 << 5); 
	
}
*/

void Timer_PWM_high_level_counter(TIM_TypeDef *timer, int CCR)
{
	if (timer == TIM4) {
		timer->CCR3 = CCR;
	} else if (timer == TIM1) {
		timer->CCR1 = CCR;
	}
}
