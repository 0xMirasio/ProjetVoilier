#ifndef _GIROUETTE_H
#define _GIROUETTE_H

#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges
#include "stm32f1xx_ll_tim.h" 

float girouette_get_angle(void); //Récypère l'angle de la girouette
void girouetteConf(void);//Configure le Timer et le GPIO
void EXTI9_5_IRQnHandler(void);//Avertit pour le zéro absolu lors d'un tour complet
void startTimer(TIM_TypeDef* TIMx);//Active le compteur
void stopTimer(TIM_TypeDef* TIMx);//Stop le compteur


#endif 
