#include "Accelerometre.h"
#include <math.h> 

#define Sensitivity 480
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

ADC_TypeDef * adc=ADC1;
//ADC init afin d'obtenir les données d'accélèromètre
void Adc_Conf_ACC(ADC_TypeDef * adc_arg) {
	adc=adc_arg;
	// on va utiliser PB0 car c'est la pin associé au channels 8 de l'adc 1 (Doc : datasheet STM32f103rb page 29)
	LL_ADC_Disable(adc); // on disable l'adc
	
	// pas sur de ça, l'adc doit avoir F<14Mhz, donc Fhorloge/4. A testez si jamais l'adc ne marche 
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); // activation horloge interne adc1
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC); // activaiton horloge gpioB

	LL_ADC_InitTypeDef ADC_InitStruct;
	LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
		
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
	
	ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
	
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
	
	LL_ADC_REG_Init(adc, &ADC_REG_InitStruct);
	LL_ADC_Init(adc, &ADC_InitStruct);
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(adc), &ADC_CommonInitStruct);
	
	// config gpio de pC0	
	LL_GPIO_InitTypeDef gp0;
		
	gp0.Pin = LL_GPIO_PIN_0;
	gp0.Mode = LL_GPIO_MODE_ANALOG;
	gp0.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gp0.Pull = LL_GPIO_PULL_UP;
	
	LL_GPIO_Init(GPIOC, &gp0);
	
	// config gpio de pC0	
	LL_GPIO_InitTypeDef gp1;
		
	gp1.Pin = LL_GPIO_PIN_1;
	gp1.Mode = LL_GPIO_MODE_ANALOG;
	gp1.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gp1.Pull = LL_GPIO_PULL_UP;
	
	LL_GPIO_Init(GPIOC, &gp1);

	LL_ADC_StartCalibration(adc); // on calibre l'adc
	//while (LL_ADC_IsCalibrationOnGoing(ADC1) == 0) {} // on attend la fin de la calibration
	LL_ADC_Enable(adc); // activation ADC
	//channel 11 -> y 
	LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_11);
	//channel 10 -> x
	//LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_10);
	
}

/*
1.3. Système anti chavirement
Si le voilier prend un angle de roulis supérieur à 40° (angle entre la verticale et la quille) alors les
voiles sont immédiatement relâchées pour que le voilier revienne à une position horizontale.
*/
float getAngle() {
	//recuperer le 
	float vol=-1;

	
	if (LL_ADC_IsEnabled(adc) == 1) { // si l'adc est activé
		uint16_t y = 1;
		LL_ADC_REG_StartConversionSWStart(adc);
		y = LL_ADC_REG_ReadConversionData12(adc);
		vol=y/4095.*3300.;
	}
	return vol;
	
}

//sevo-moteur
//quand l'angle est > 40, alors les
//voiles sont immédiatement relâchées  
void bougerVoile(float angle){
	
//faire une pwm envoyé au servo-moteur

//utiliser timer1 sur PA8 pour faire pwm
//mettre PA8 en output car de là sort la pwm initialisée en timer CH1 
//TIM1 CCR1 
	if (angle > 1738) { 

		int CCR1Max = 6545;  //ARR*0,1 = 2 ms -> 90  angle ->voiles relâchées 
		LL_TIM_OC_SetCompareCH1(TIM1, CCR1Max);
		
	}
	
}
//timer initialisation pour le pwm 
void timer_pwm_init(){
	//72000 * 2 = 144000 
	//TIM1 CCR1 ARR -> 16 bits < 65535
	int Psc =21;
	int Arr = 65454; 
	LL_TIM_InitTypeDef Timer;
	
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	
	Timer.Autoreload= Arr ;
	Timer.Prescaler= Psc;
	Timer.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	Timer.CounterMode=LL_TIM_COUNTERMODE_UP;
	Timer.RepetitionCounter=0;
	
	LL_TIM_Init(TIM1,&Timer); //TIM1
	
	LL_TIM_OC_SetMode (TIM1, LL_TIM_CHANNEL_CH1,LL_TIM_OCMODE_PWM1);
	TIM1->BDTR |= TIM_BDTR_MOE; //uniquement pour TIM 1 et 8
	
	TIM1->CCER |= TIM_CCER_CC1E;
	
	
	LL_TIM_EnableCounter(TIM1);

}

//gpio initialisation pour le pwm 
void gpio_servom_init() {

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	LL_GPIO_InitTypeDef pa8;
	pa8.Pin = LL_GPIO_PIN_8;  //PA8
	pa8.Mode = LL_GPIO_MODE_ALTERNATE;
	pa8.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa8.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa8.Pull = LL_GPIO_PULL_UP ;//Vcc

	LL_GPIO_Init(GPIOA, &pa8);
}



