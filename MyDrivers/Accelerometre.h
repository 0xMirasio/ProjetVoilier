#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"

void Adc_Conf_ACC(ADC_TypeDef * adc);

float getAngle(void);

void bougerVoile(float angle);

void timer_pwm_init(void);

void gpio_servom_init(void);
