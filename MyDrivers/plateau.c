#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h" //Utilisation sur l'activation des horloges
#include "stm32f1xx_ll_tim.h" //

#include "plateau.h"

// RF_teleco sur PB6 PB7
void gpio_RF_init(){
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB); //PB GPIOB
	//period PB6
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_FLOATING); //PB6 
	//pulse PB7
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_FLOATING); //PB7
}

//orientation du voilier 
/*L'utilisateur peut donner n'importe quel cap au bateau en utilisant une télécommande.
Concrètement, le disque qui supporte le bateau peut tourner vers la droite ou la gauche avec une
vitesse réglable de 0 à 100% dans les deux sens.*/
float vitesse;
int sens;

void timer_RF_init(){ // TIM4
	//init timer
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4); //PB6 correspond au TIM4_CH1; PB7 correspond au TIM4_CH2; 
	LL_TIM_InitTypeDef timer_init; //structure timer
	timer_init.Autoreload = 19999; //16bits 20000 * 72 = f pwm ;; pwm = 20ms = 144 mhz
	timer_init.Prescaler = 71; //1 000 000 Ticks/s = (72MHz/(psc+1)) 16bits = 10ms * 2 = 20 ms = 144MHZ
	timer_init.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;//clock division : 00 Tdts=Tck_int
	timer_init.CounterMode=LL_TIM_COUNTERMODE_UP;
	timer_init.RepetitionCounter=0;
	
	LL_TIM_Init(TIM4,&timer_init);
	
	//Configurer PWM en input mode P315 BIT MAP P362 
	TIM4->CCMR1 &= ~(0x0003);
	TIM4->CCMR1 |= 0x0001;  // write the CC1S bits to 01 in the TIMx_CCMR1 register
	TIM4->CCER &= ~(0x0002); // write the CC1P to ‘0’  (active on rising edge)
	TIM4->CCMR1 &= ~(3<<8); // write the CC2S bits to 10 in the TIMx_CCMR1 register (TI1 selected).
	TIM4->CCMR1 |= 2<<8;
	TIM4->CCER |= 1<<5; // write the CC2P bit to ‘1’ (active on falling edge)
	TIM4->SMCR &= ~(7<<4); // write the TS bits to 101 in the TIMx_SMCR register (TI1FP1 selected).
	TIM4->SMCR |= 5<<4;
	TIM4->SMCR &= ~(0x0007); // write the SMS bits to 100 in the TIMx_SMCR register.
	TIM4->SMCR |= 0x0004;
	TIM4->CCER |= 0x0001; // Enable the captures: write the CC1E and CC2E bits to ‘1 in the TIMx_CCER register
	TIM4->CCER |= 1<<4;
	
	LL_TIM_EnableCounter(TIM4);	// Active le TIM4
	
	vitesse = 0.;//float avec " . " 
	sens = 0;// 1 ou 0	
}


float get_vitesse_sens(){
	int pulse = LL_TIM_OC_GetCompareCH2(TIM4) +1; // get compare value(TIMx_CCR2) set for output channel2
	int period = LL_TIM_OC_GetCompareCH1(TIM4) +1;
	vitesse = ((float)(pulse)/1000.)*2. -3.; //vitesse entre -1 et 1//duree de l'impulsion entre 1ms et 2ms donc pulse  entre 1000 et 2000
	//vitesse=-0.;	
	if (vitesse>=0.){
		sens=1;
	}
	else{
		sens=0;
	}
	return vitesse;
	
}


void gpio_mcc_init() {
	
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);  //GPIOA sur APB2
	//configuration de la PA1 pour générer la PWM //pull-up
	LL_GPIO_InitTypeDef pa1; // pwm PA1 tim 2 ch2
	pa1.Pin = LL_GPIO_PIN_1;
	pa1.Mode = LL_GPIO_MODE_ALTERNATE;
	pa1.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa1.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa1.Pull = LL_GPIO_PULL_UP ;
	
	LL_GPIO_Init(GPIOA, &pa1);
	
	//configuration de la PA2 qui gère le sens (en 0 ou 1)
	LL_GPIO_InitTypeDef pa2; // sens PA2 tim 2 ch3
	pa2.Pin = LL_GPIO_PIN_2;
	pa2.Mode = LL_GPIO_MODE_OUTPUT;
	pa2.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa2.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa2.Pull = LL_GPIO_PULL_UP ;
	
	LL_GPIO_Init(GPIOA, &pa2);
}		

	
void timer_pwm_mcc_init() { //TIM2
	
	int Arr = 65454 ; //max pour 16 bits
	int Psc = 54 ; // 72 000 000 / 65454*(54+1) = 20ms pour timer

	LL_TIM_InitTypeDef Timer;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	//configurer le structure timer
	Timer.Autoreload= Arr;//timer correspondant à la PWM (TIM2)
	Timer.Prescaler= Psc;
	Timer.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	Timer.CounterMode=LL_TIM_COUNTERMODE_UP;
	Timer.RepetitionCounter=0;
	LL_TIM_Init(TIM2,&Timer);
	LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2,LL_TIM_OCMODE_PWM1); // Define the behavior of the output reference signal derived.
	TIM2->CCER |= TIM_CCER_CC2E;
	LL_TIM_EnableCounter(TIM2);
		
}	

float absolu(float a){
	if (a<0){
		a=-a;
	}
	return(a);
}

int i=0;
	
void bougerPlateau() {
	float vitesse2;
	int vit2;
	//sens = -1; // changer to 1/-1 pour debug la rotation du plateau si jamais pb de capteur
	if (sens) { 
		GPIOA -> ODR &= ~0x00000004 ; //pin2 correspond au ODR3
	}
	else  //dans l'autre sens
	{
		GPIOA -> ODR |= 0x00000004 ;
	}	
	vitesse2=absolu(vitesse); // on recoit une vitesse entre -1 et 1 et on fait un seuil entre -0.2 et 0.2 où c'est stable
	if (vitesse2<0.2) {
		vitesse2 = 0.0;
	}
	else {
		 vitesse2=((vitesse2-0.2)/0.8)*65454.0 ;//vitesse à convertir en pourcentage //  Arr=65454 //2339162.5
	}

	vit2 = (int)(vitesse2) ;
	//vit2 = 30000; // changer pour debug la vitesse de la rotation si jamais capteur pb.
	LL_TIM_OC_SetCompareCH2(TIM2, vit2);   // Set compare value for output channel2 /bouger voilier
}
