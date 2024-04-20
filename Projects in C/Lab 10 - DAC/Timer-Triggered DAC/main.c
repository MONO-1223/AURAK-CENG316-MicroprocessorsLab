#include "stm32l476xx.h"
#include "LCD.h"
#include "DAC.h"
#include "TIM.h"
#include "LED.h"
#include "SysTimer.h"
#include "SysClock.h"

volatile uint32_t i;

int main(void){
	
	i = 0;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	SysTick_Init();

	LED_Init();
	
	LCD_PIN_Init();
	LCD_Clock_Init();	
	LCD_Configure();
	LCD_Clear();
	LCD_DisplayString((uint8_t*)"MONO");
	LCD_bar();
	
	//TIM4_TRGO triggers DAC.
	// GPIO PB6 (TIM4_CH1) is outputed for debugging
	TIM4_Init();
	
	
	// Analog Outputs: PA5 (DAC1_OUT2)
	DAC_Init();
	
	while(1){
		//while(Microphone_DMA_Done == 0);
		GPIOD->ODR |= GPIO_ODR_ODR_0;   // Set PD 0 as high
		delay(500);
		GPIOD->ODR &= ~GPIO_ODR_ODR_0;  // Set PD 0 as low
		// The duty ratio of the signal on PD 0 represents CPU utilization
	}
}

void TIM4_IRQHandler(void){	
	// Clear interrupt flags
	TIM4->SR = 0;
	DAC->DHR12R2 = (i++) % 4096;
}


void TIM6_DAC_IRQHandler(void){
	// Clear interrupt flags
}

