#include "DAC.h"
#include "LED.h"
#include "SysTimer.h"
#include "SysTimer.h"

#include "stm32l476xx.h"
#include <stdint.h>

// PA.5 = DAC1_OUT2

// ******************************************************************************************
// DAC Initialization
// ******************************************************************************************
void DAC_Init(void){
	
	DAC_Pin_Configuration();

	DAC_Configuration();
	
	// Enable DAC interruptions
  // NVIC_SetPriority(TIM6_DAC_IRQn, 0);
  // NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	// OPAMP_Init();
}
	
// ******************************************************************************************
// DAC Pin Initialization
// ******************************************************************************************
void DAC_Pin_Configuration(void){
	// Enable the clock of GPIO Port A
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;

	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// Configure PA5 (DAC1_OUT2) as Analog
	GPIOA->MODER |=   3U<<(2*5);  // Mode 11 = Analog
	GPIOA->PUPDR &= ~(3U<<(2*5)); // No pull-up, no pull-down	
	
	// GPIO port analog switch control register (ASCR)
	// 0: Disconnect analog switch to the ADC input (reset state)
	// 1: Connect analog switch to the ADC input
	GPIOA->ASCR |= GPIO_ASCR_EN_5;
}


// ******************************************************************************************
// DAC Configuration
// ******************************************************************************************
void DAC_Configuration(void){
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;  // Enable DAC Clock
	
	DAC_Calibration_Channel(2);  // Calibrate DAC Channel 2 because it is related with PA5 which is available on our board
	// Unlike channel 1 which we cann't use because it is connected to a pin that is not available on our board

	// DAC mode control register (DAC_MCR)
	// - DAC Channel 2 in normal Mode
	//  000: DAC Channel 2 is connected to external pin with Buffer enabled
	//  001: DAC Channel 2 is connected to external pin and to on chip peripherals with buffer enabled
	//  010: DAC Channel 2 is connected to external pin with buffer disabled
	//  011: DAC Channel 2 is connected to on chip peripherals with Buffer disabled
	// - DAC Channel 2 in sample & hold mode
	//  100: DAC Channel 2 is connected to external pin with Buffer enabled
	//  101: DAC Channel 2 is connected to external pin and to on chip peripherals with Buffer enabled
	//  110: DAC Channel 2 is connected to external pin and to on chip peripherals with Buffer disabled
	//  111: DAC Channel 2 is connected to on chip peripherals with Buffer disabled
	DAC->MCR &= ~DAC_MCR_MODE2; //Page 628   // 000: DAC Channel 2 is connected to external pin with buffer enabled
	
	// DAC channel2 trigger enable
	DAC->CR |=  DAC_CR_TEN2;     //page 637  // Trigger enable 
	// DAC Trigger selection
	// 000: Timer 6 TRGO event
	// 001: Timer 8 TRGO event
	// 010: Timer 7 TRGO event
	// 011: Timer 5 TRGO event
	// 100: Timer 2 TRGO event
	// 101: Timer 4 TRGO event
	// 110: External line9
	// 111: Software trigger	
	DAC->CR |=  DAC_CR_TSEL2;     //page 625 Table 123. DAC trigger selection      // Software Trigger
	
	// Select 101 TIM4_TRGO as DAC triggers for Channel 2
	// DAC->CR |= ( DAC_CR_TSEL2_0 | DAC_CR_TSEL2_2); 
		
	DAC->CR |=  DAC_CR_EN2;       // Enable DAC Channel 2
	
	delay(1);
}

// ******************************************************************************************
// DAC Calibration
// ******************************************************************************************
void DAC_Calibration_Channel(uint32_t channel){
	
	uint32_t trimmingvalue, delta, offset;
	uint32_t DAC_CR_CEN_Flag, DAC_CCR_OTRIM_Flag, DAC_SR_CAL_Flag;
	
	if (channel == 1) {
		DAC_CR_CEN_Flag = DAC_CR_CEN1;
		DAC_CCR_OTRIM_Flag = DAC_CCR_OTRIM1;
		DAC_SR_CAL_Flag = DAC_SR_CAL_FLAG1;
		offset = 0;
	} else {
		DAC_CR_CEN_Flag = DAC_CR_CEN2;
		DAC_CCR_OTRIM_Flag  = DAC_CCR_OTRIM2;
		DAC_SR_CAL_Flag = DAC_SR_CAL_FLAG2;
		offset = 16;
	}
	
	if (channel == 1) {
		DAC->CR &= ~DAC_CR_EN1;  // Ensure DAC 1 is off
	} else {
		DAC->CR &= ~DAC_CR_EN2;  // Ensure DAC 2 is off
	}
	
	// Enable DAC Channel calibration 
	DAC->CR |=  DAC_CR_CEN_Flag;  
	
	/* Init trimming counter */    
  /* Medium value */
  trimmingvalue = 16; 
  delta = 8;
  while (delta != 0) {
		
    /* Set candidate trimming */
		// DAC calibration control register (DAC_CCR)
		DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
		DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
  
    /* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
    /* i.e. minimum time needed between two calibration steps */
    delay(1);
  
		if ((DAC->SR & DAC_SR_CAL_Flag) == 0) 
			/* DAC_SR_CAL_FLAGx is HIGH, try higher trimming */
			trimmingvalue += delta;
		else
			trimmingvalue -= delta;
		   
		delta >>= 1;
	}
	
	/* Still need to check if right calibration is current value or one step below */
	/* Indeed the first value that causes the DAC_SR_CAL_FLAGx bit to change from 0 to 1  */
	/* Set candidate trimming */
	DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
	DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
  
	/* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
	/* i.e. minimum time needed between two calibration steps */
  delay(1);
    
	if ((DAC->SR & DAC_SR_CAL_Flag) == 0) { 
		/* OPAMP_CSR_OUTCAL is actually one value more */
		trimmingvalue++;
		/* Set right trimming */
		DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
		DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
	}
	
	DAC->CR &= ~DAC_CR_CEN_Flag; 
}
