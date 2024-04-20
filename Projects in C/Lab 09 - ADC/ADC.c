#include "ADC.h"
#include "LED.h"
#include "SysTimer.h"
#include "stm32l476xx.h"
#include <stdint.h>

// Analog Inputs: 
//    PA1 (ADC12_IN6), PA2 (ADC12_IN7)
//    These pins are not used: PA0 (ADC12_IN5, PA3 (ADC12_IN8)

// ******************************************************************************************
// STM32L4x6xx Errata sheet
// When the delay between two consecutive ADC conversions is higher than 1 ms the result of 
// the second conversion might be incorrect. The same issue occurs when the delay between the 
// calibration and the first conversion is higher than 1 ms.
// Workaround
// When the delay between two ADC conversions is higher than the above limit, perform two ADC 
// consecutive conversions in single, scan or continuous mode: the first is a dummy conversion 
// of any ADC channel. This conversion should not be taken into account by the application.

// ******************************************************************************************
// ADC Wakeup
// By default, the ADC is in deep-power-down mode where its supply is internally switched off
// to reduce the leakage currents.
// ******************************************************************************************
void ADC_Wakeup (void) {
	
	int wait_time;
	
	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state
	
	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launching a calibration or enabling the ADC, the ADC
	// voltage regulator must first be enabled and the software must wait for the regulator start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;	
	
	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP) 
	// before launching a calibration or enabling the ADC.
	// T_ADCVREG_STUP = 20 us
	wait_time = 20 * (80000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}   
}

// ******************************************************************************************
// 	ADC Common Configuration
// ******************************************************************************************	
void ADC_Common_Configuration(){
	
	// I/O analog switches voltage booster
	// The I/O analog switches resistance increases when the VDDA voltage is too low. This
	// requires to have the sampling time adapted accordingly (cf datasheet for electrical
	// characteristics). This resistance can be minimized at low VDDA by enabling an internal
	// voltage booster with BOOSTEN bit in the SYSCFG_CFGR1 register.
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	
	// V_REFINT enable
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;  
	
	// ADC Clock Source: System Clock, PLLSAI1, PLLSAI2
	// Maximum ADC Clock: 80 MHz
	
	// ADC prescaler to select the frequency of the clock to the ADC
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;   // 0000: input ADC clock not divided
	
	// ADC clock mode
	//   00: CK_ADCx (x=123) (Asynchronous clock mode),
	//   01: HCLK/1 (Synchronous clock mode).
	//   10: HCLK/2 (Synchronous clock mode)
	//   11: HCLK/4 (Synchronous clock mode)	 
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;  // HCLK = 80MHz
	ADC123_COMMON->CCR |=  ADC_CCR_CKMODE_0;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Independent Mode
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
	ADC123_COMMON->CCR |= 6U;  // 00110: Regular simultaneous mode only
}


// ******************************************************************************************
// 	ADC Pin Initialization
//  PA1 (ADC12_IN6), PA2 (ADC12_IN7)
// ******************************************************************************************
void ADC_Pin_Init(void){	
	// Enable the clock of GPIO Port A
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;
	
	// GPIO Pin Initialization
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	// Configure PA1 (ADC12_IN6), PA2 (ADC12_IN7) as Analog
	GPIOA->MODER |=  3U<<(2*1) | 3U<<(2*2);  // Mode 11 = Analog
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR &= ~( 3U<<(2*1) | 3U<<(2*2)); // No pull-up, no pull-down
	
	// GPIO port analog switch control register (ASCR)
	// 0: Disconnect analog switch to the ADC input (reset state)
	// 1: Connect analog switch to the ADC input
	GPIOA->ASCR |= GPIO_ASCR_EN_1 | GPIO_ASCR_EN_2;
}

// ******************************************************************************************
// Initialize ADC	
// ******************************************************************************************	
void ADC_Init(void){
	
	
	// Enable the clock of ADC
	RCC->AHB2ENR  |= RCC_AHB2ENR_ADCEN;
	RCC->AHB2RSTR	|= RCC_AHB2RSTR_ADCRST;
	(void)RCC->AHB2RSTR; // short delay
	RCC->AHB2RSTR	&= ~RCC_AHB2RSTR_ADCRST;
	
	ADC_Pin_Init();
	ADC_Common_Configuration();
	ADC_Wakeup();
	
	
	// ADC control register 1 (ADC_CR1)
	// L1: ADC1->CR1			&= ~(ADC_CR1_RES);							// 
	ADC1->CFGR &= ~ADC_CFGR_RES;     	// Resolution, (00 = 12-bit, 01 = 10-bit, 10 = 8-bit, 11 = 6-bit)
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;   	// Data Alignment (0 = Right alignment, 1 = Left alignment)
		
	// L1: ADC1->CR1			&= ~(ADC_CR1_SCAN);							// Scan mode disabled
	
	// ADC regular sequence register 1 (ADC_SQR1)
	// L1: ADC1->SQR1 		&= ~ADC_SQR1_L; 							  // 00000: 1 conversion in the regular channel conversion sequence
	ADC1->SQR1 &= ~ADC_SQR1_L;            // 0000: 1 conversion in the regular channel conversion sequence
	
	// Specify the channel number of the 1st conversion in regular sequence
	// L1: ADC1->SQR5 		|= (5 & ADC_SQR5_SQ1);	// SQ1[4:0] bits (1st conversion in regular sequence)					
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
	ADC1->SQR1 |=  ( 6U << 6 );           	// PA1: ADC12_IN6 
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6; 	// Single-ended for PA1: ADC12_IN6 
	
	// ADC Sample Time
	// This sampling time must be enough for the input voltage source to charge the embedded
	// capacitor to the input voltage level.
	// Software is allowed to write these bits only when ADSTART=0 and JADSTART=0
	//   000: 2.5 ADC clock cycles      001: 6.5 ADC clock cycles
	//   010: 12.5 ADC clock cycles     011: 24.5 ADC clock cycles
	//   100: 47.5 ADC clock cycles     101: 92.5 ADC clock cycles
	//   110: 247.5 ADC clock cycles    111: 640.5 ADC clock cycles	
	
	// ADC_SMPR3_SMP5 = Channel 5 Sample time selection
	// L1: ADC1->SMPR3 		&= ~ADC_SMPR3_SMP5;		// sample time for first channel, NOTE: These bits must be written only when ADON=0. 
	ADC1->SMPR1  &= ~ADC_SMPR1_SMP6;      // ADC Sample Time
	ADC1->SMPR1  |= 3U << 18;             // 3: 24.5 ADC clock cycles @80MHz = 0.3 us
	
	// ADC control register 2 (ADC_CR2)
	// L1: ADC1->CR2 			&=  ~ADC_CR2_CONT;    // Disable Continuous conversion mode		
	ADC1->CFGR &= ~ADC_CFGR_CONT;               // ADC Single/continuous conversion mode for regular conversion		
	
	// L1: NVIC_SetPriority(ADC1_IRQn, 1); // Set Priority to 1
	// L1: NVIC_EnableIRQ(ADC1_IRQn);      // Enable interrupt form ACD1 peripheral
	
	// L1: ADC1->CR1 		  |= ADC_CR1_EOCIE; 							// Enable interrupt: End Of Conversion
	// ADC1->IER |= ADC_IER_EOC;  // Enable End of Regular Conversion interrupt
	// ADC1->IER |= ADC_IER_EOS;            // Enable ADC End of Regular Sequence of Conversions Interrupt		
	// NVIC_EnableIRQ(ADC1_2_IRQn);
	
	// Configuring the trigger polarity for regular external triggers
	// 00: Hardware Trigger detection disabled, software trigger detection enabled
	// 01: Hardware Trigger with detection on the rising edge
	// 10: Hardware Trigger with detection on the falling edge
	// 11: Hardware Trigger with detection on both the rising and falling edges
	ADC1->CFGR &= ~ADC_CFGR_EXTEN; 
	
	// Enable ADC1
	// L1: ADC1->CR2  |= ADC_CR2_ADON;     // Turn on conversion	
	ADC1->CR |= ADC_CR_ADEN;  
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0); 
	
	// L1: ADC1->CR2  |= ADC_CR2_CFG;       // ADC configuration: 0: Bank A selected; 1: Bank B selected
	// L1: ADC1->CR2	|= ADC_CR2_SWSTART;		// Start Conversion of regular channels	
	// L1: while(ADC1->CR2 & ADC_CR2_CFG);	// Wait until configuration completes			
}


// ******************************************************************************************
// 	ADC 1/2 Interrupt Handler
// ******************************************************************************************
void ADC1_2_IRQHandler(void){
	NVIC_ClearPendingIRQ(ADC1_2_IRQn);
	
	// ADC End of Conversion (EOC)
	if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC) {
		// It is cleared by software writing 1 to it or by reading the corresponding ADCx_JDRy register
		ADC1->ISR |= ADC_ISR_EOC;
	}
	
	// ADC End of Injected Sequence of Conversions  (JEOS)
	if ((ADC1->ISR & ADC_ISR_EOS) == ADC_ISR_EOS) {
		// It is cleared by software writing 1 to it.
		ADC1->ISR |= ADC_ISR_EOS;		
	}
}