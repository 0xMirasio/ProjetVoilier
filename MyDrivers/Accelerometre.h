#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"

//configurer ADC
void Adc_Conf_ACC(ADC_TypeDef * adc);

//obtenir angle
float getAngle(void);

//manipuler le servo-moteur selon l'angle
void bougerVoile(float angle);

//initialiser TIM pour servo-moteur
void timer_pwm_init(void);

//initialiser GPIO pour servo-moteur
void gpio_servom_init(void);
