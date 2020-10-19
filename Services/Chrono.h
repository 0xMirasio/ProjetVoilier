// RIEN A MODIFIER //

#ifndef CHRONO_H
#define CHRONO_H

/*
Service permettant de chornométrer jusqu'à 59mn 59s 99 1/100
Utilise un timer au choix (TIMER1 à TIMER4).
Utilise la lib MyTimers.h /.c
*/

#include "stm32f103xb.h" 

/* =====================================================================================
Les fonctions qui gèrent les IO (ajout par rapport à l'activité 1)
=======================================================================================*/


#define PinStart LL_GPIO_PIN_8
#define PinStop LL_GPIO_PIN_6
// à compléter pour la pin reset du chronometre
#define PinLED LL_GPIO_PIN_10
#define GPIO_Pin GPIOC
#define GPIO_LED GPIOC






/**
	* @brief  polle les 3 entrées et gènère les actions à faire
  * @note   Fct à lancer en tâche de fond (non bloquante)
	* @param  None 
  * @retval None
  */
		
void Chrono_Background(void);

//=======================================================================================//




// Type de donnée utilisé dans le module

typedef struct {
	char Hund;
	char Sec;
	char Min;
} Time;




/**
	* @brief  Configure le chronomètre. 
  * @note   A lancer avant toute autre fonction.
	* @param  TIM_TypeDef Timer : indique le timer à utiliser par le chronomètre, TIM1, TIM2, TIM3 ou TIM4
  * @retval None
  */
void Chrono_Conf(TIM_TypeDef * Timer);






/**
	* @brief  Démarre le chronomètre. 
  * @note   si la durée dépasse 59mn 59sec 99 cent, elle est remise à zéro et repart
	* @param  Aucun
  * @retval Aucun
  */
void Chrono_Start(void);


/**
	* @brief  Arrête le chronomètre. 
  * @note   
	* @param  Aucun
  * @retval Aucun
  */
void Chrono_Stop(void);


/**
	* @brief  Remet le chronomètre à 0 
  * @note   
	* @param  Aucun
  * @retval Aucun
  */
void Chrono_Reset(void);


/**
	* @brief  Renvoie l'adresse de la variable Time privée gérée dans le module Chrono.c
  * @note   
	* @param  Aucun
  * @retval adresse de la variable Time
  */
Time * Chrono_Read(void);




#endif
