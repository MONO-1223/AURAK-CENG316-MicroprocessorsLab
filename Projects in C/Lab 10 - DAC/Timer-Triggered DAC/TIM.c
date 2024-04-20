#include "TIM.h"
#include "LED.h"
#include "stm32l476xx.h"
#include <stdint.h>

// ******************************************************************************************
// GPIO PB6 as TIM4_CH1 for ADC and DAC triggers		
// ******************************************************************************************
void TIM4_Init(void){
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN; // Enable Clock of Timer 4
	
	TIM4->CR1  &= ~TIM_CR1_CMS;  // Edge-aligned mode
	TIM4->CR1  &= ~TIM_CR1_DIR;  // Counting direction: Up Counting
 	
	// Master mode selection
	// 000: UG bit from the TIMx_EGR register is used as trigger output (TRGO). 
	// 001: Enable - the Counter Enable signal CNT_EN is used as trigger output (TRGO). 
	// 010: Update - The update event is selected as trigger output (TRGO). 
	// 011: Compare Pulse - The trigger output send a positive pulse when the CC1IF flag is to be set (even if it was already high).
	// 100: Compare - OC1REF signal is used as trigger output (TRGO)
	// 101: Compare - OC2REF signal is used as trigger output (TRGO)
	// 110: Compare - OC3REF signal is used as trigger output (TRGO)
	// 111: Compare - OC4REF signal is used as trigger output (TRGO)	
	TIM4->CR2  &= ~TIM_CR2_MMS;  // Master mode selection  //page 1059
	TIM4->CR2  |= TIM_CR2_MMS_2; // 100 = OC1REF as TRGO 

	TIM4->DIER |= TIM_DIER_TIE;  // Trigger interrupt enable //page 1063
	
	TIM4->DIER |= TIM_DIER_UIE;  // Update interrupt enable //page 1064
	
	// OC1M: Output Compare 1 mode
	// 0110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT < TIMx_CCR1
  // else inactive. In downcounting, channel 1 is inactive (OC1REF = '0') as long as
	// TIMx_CNT > TIMx_CCR1 else active (OC1REF = '1').
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M; //page 1066
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;  // 0110 = PWM mode 1  //page 1068
	
	// The counter clock frequency (CK_CNT) = fCK_PSC / (PSC[15:0] + 1)
	// Timer driving frequency = 80 MHz/(1 + PSC) = 80 MHz/(1+7) = 10MHz
	TIM4->PSC  =   7;    // max 65535
	
  // Trigger frequency = 10MHz / (1 + ARR) = 10MHz/1000 = 10KHz
	TIM4->ARR  = 999;   // max 65535
	
	TIM4->CCR1 = 500;   // Duty ration 50%
	 
	TIM4->CCER |= TIM_CCER_CC1E;  //  OC1 signal is output on the corresponding output pin
	
	// Enable timer
	TIM4->CR1  |= TIM_CR1_CEN;   // Enable counter
	
	// Set up GPIO pin PB6 as TIM4_CH1N for debugging
	RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER  &= ~(3U<<(2*6));
	GPIOB->MODER  |=   2U<<(2*6);    // Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->AFR[0] &= ~0x0F000000; 
	GPIOB->AFR[0] |=  0x02000000;    // AF2 = TIM4_CH1N for PB6	
}

