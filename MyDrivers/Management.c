#include "Management.h"

#define separator 0x3a // 0x3a = :
#define retour_chariot 0x0D // 0x0D = \r 
#define retour_ligne 0x0a // \n
#define space 32
#define VDDA_APPLI	((uint32_t)3300) // Value of analog reference voltage (Vref+), connected to analog voltage supply Vdda (unit: mV). 


void Usart_conf(USART_TypeDef * ux) {
		LL_USART_Disable(ux); // on desactive l'usart
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2); // activation horloge interne usart
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA); // activaiton horloge gpioA
	
		LL_GPIO_InitTypeDef gp;
		
		gp.Pin = LL_GPIO_PIN_2;
		gp.Mode = LL_GPIO_MODE_ALTERNATE;
		gp.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		gp.Speed = LL_GPIO_SPEED_FREQ_HIGH;
		gp.Pull = LL_GPIO_PULL_UP;
	
		LL_GPIO_Init(GPIOA, &gp); // pin TX = 2
	
		LL_USART_InitTypeDef us;
		us.BaudRate = 9600;
		us.HardwareFlowControl =	LL_USART_HWCONTROL_NONE; 
		us.TransferDirection = LL_USART_DIRECTION_TX; 
		us.Parity = LL_USART_PARITY_NONE; // pas de parité
		us.StopBits = LL_USART_STOPBITS_1; // 1 bit de stop
		us.DataWidth = LL_USART_DATAWIDTH_8B; // 8b de données
		LL_USART_Init(ux, &us); // init de la struct
		
		LL_USART_EnableIT_RXNE(ux); 
		LL_USART_EnableIT_TC(ux);
		LL_USART_Enable(ux); // activation usart
}

void Adc_Conf(ADC_TypeDef * adc) {
	
	// on va utiliser PB0 car c'est la pin associé au channels 8 de l'adc 1 (Doc : datasheet STM32f103rb page 29)
	LL_ADC_Disable(adc); // on disable l'adc
	
	// pas sur de ça, l'adc doit avoir F<14Mhz, donc Fhorloge/4. A testez si jamais l'adc ne marche 
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); // activation horloge interne adc1
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB); // activaiton horloge gpioB

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
	
	LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
	LL_ADC_Init(adc, &ADC_InitStruct);
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
	
	// config gpio de pb0	
	LL_GPIO_InitTypeDef gp;
		
	gp.Pin = LL_GPIO_PIN_0;
	gp.Mode = LL_GPIO_MODE_ANALOG;
	gp.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gp.Pull = LL_GPIO_PULL_UP;
	
	LL_GPIO_Init(GPIOB, &gp);

	LL_ADC_StartCalibration(ADC1); // on calibre l'adc
	while (LL_ADC_IsCalibrationOnGoing(ADC1) == 0) {} // on attend la fin de la calibration
	LL_ADC_Enable(adc); // activation usart
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_8);
	
}

void usart_sendClearMessage(USART_TypeDef * ux, uint16_t voltage, Time * Chrono_Time) {
	int message[38] = {69,118,101,114,121,116,104,105,110,103,32,79,75,44,32,66,97,116,116,101,114,121,32,83,116,97,116,101,32,108,101,118,101,108,32,97,116,32};
	// message = 'Everything OK, Battery State level at : XXXX mV '
	send_time(ux, Chrono_Time);
	for (int i=0; i < 38 ; i++) {
					send_char(ux, message[i]);
	}
	
	int voltage1 = (int)(voltage / 1000);
	voltage = voltage % 1000;
	int voltage2 = (int)(voltage / 100);
	voltage = voltage % 100;
	int voltage3 = (int)(voltage / 10);
	voltage = voltage % 10;
	send_char(ux, separator);
	send_char(ux, 48 + voltage1);
	send_char(ux, 48 + voltage2);
	send_char(ux, 48 + voltage3);
	send_char(ux, 48 + voltage);
	send_char(ux, space);
	send_char(ux, 109); // m
	send_char(ux, 86); // V

	for (int i = 0; i<20 ; i++) {
					send_char(ux, 45);
	}
	
	send_char(ux, retour_chariot); // \r
	
	
}
void send_time(USART_TypeDef * ux, Time * time) {
	send2b(ux, time->Hour);
	send_char(ux, separator);
	send2b(ux, time->Min);
	send_char(ux, separator);
	send2b(ux, time->Sec);
	send_char(ux, separator);
	send2b(ux, time->Hund);
	send_char(ux, separator);
	send_char(ux, 45); // -
	send_char(ux,62); // >
	send_char(ux, space);
}

void send2b(USART_TypeDef * ux,  int toSend) {
	LL_USART_TransmitData8(ux,(toSend/10)+0x30); // on send l'info + 0x30 (conversion table ascii)
	while(LL_USART_IsActiveFlag_TC(ux) == 0) {} // on att la transmission du bit 1
	LL_USART_TransmitData8(ux,(toSend%10)+0x30);
	while(LL_USART_IsActiveFlag_TC(ux) == 0) {} // on att la transmission du bit 2
}
void Usart_sendBatInfo(USART_TypeDef * ux, uint16_t voltage, Time * Chrono_Time) {
		int message[55] = {84,104,101,114,101,32,105,115,32,97,32,112,114,111,98,108,101,109,32,119,105,116,104,32,66,97,116,116,101,114,121,32,83,116,97,116,101,32,33,32,67,117,114,114,101,110,116,32,76,101,118,101,108,32,32};
			// message = 'There is a problem with Battery State ! Current Level : XXXX mV '
		send_time(ux, Chrono_Time);
		for (int i=0; i < 55 ; i++) {
					send_char(ux, message[i]);
		}
		int voltage1 = (int)(voltage / 1000);
		voltage = voltage % 1000;
		int voltage2 = (int)(voltage / 100);
		voltage = voltage % 100;
		int voltage3 = (int)(voltage / 10);
		voltage = voltage % 10;
		send_char(ux, separator);
		send_char(ux, 48 + voltage1);
		send_char(ux, 48 + voltage2);
		send_char(ux, 48 + voltage3);
		send_char(ux, 48 + voltage);
		send_char(ux, space);
		send_char(ux, 109); // m
		send_char(ux, 86); // V
		send_char(ux,retour_chariot); // \r
		
}

void send_char(USART_TypeDef * ux, int data) {
		LL_USART_TransmitData8(ux,data);
		while(LL_USART_IsActiveFlag_TC(ux) == 0) {}
}


uint16_t getAlimentationState() {
	
	uint16_t voltage = 1;
	if (LL_ADC_IsEnabled(ADC1) == 1) { // si l'adc est activé
			LL_ADC_REG_StartConversionSWStart(ADC1);
			voltage = LL_ADC_REG_ReadConversionData12(ADC1);
	}
	return voltage;

}
