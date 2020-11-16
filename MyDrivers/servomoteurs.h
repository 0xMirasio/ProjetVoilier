#ifndef _SERVOMOTEURS_H
#define _SERVOMOTEURS_H

#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges
#include "stm32f1xx_ll_tim.h" 

void servo_start(TIM_TypeDef *timer);

void servoConf(void);

void servo_setAngle(int angleGirouette);

#endif
