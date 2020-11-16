#include "stm32f1xx_ll_usart.h"
#include "stm32f103xb.h"
#include "Chrono.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_tim.h"


void Usart_conf(USART_TypeDef * ux);
uint16_t getAlimentationState(void);
void Adc_Conf(ADC_TypeDef * adc);
void send_char(USART_TypeDef * ux, int data);
void usart_sendClearMessage(USART_TypeDef * ux, uint16_t voltage, Time * Chrono_Time);
void Usart_sendBatInfo(USART_TypeDef * ux, uint16_t voltage, Time * Chrono_Time);
void send_time(USART_TypeDef * ux, Time * time);
void send2b(USART_TypeDef * ux,  int toSend);


