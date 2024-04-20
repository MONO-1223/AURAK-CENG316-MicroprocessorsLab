#include "stm32l476xx.h"
#include "LCD.h"
#include "DAC.h"
#include "LED.h"
#include "SysTimer.h"
#include "SysClock.h"


int main(void){
	unsigned int i = 0, output = 0;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	SysTick_Init();
	LED_Init();
	
	LCD_PIN_Init();
	LCD_Clock_Init();	
	LCD_Configure();
	LCD_Clear();
	LCD_DisplayString((uint8_t*)"MONO"); // change
	LCD_bar();
	

	// Analog Outputs: PA5 (DAC1_OUT2)
	//  0 <=> 0V, 4095 <=> 3.0V 
	DAC_Init();	
	
	while(1){
		
		GPIOD->ODR |= GPIO_ODR_ODR_0;   // Set PD 0 as high
		
		// Wait until not busy
		// This bit is systematically set just after Sample & Hold mode enable and is set each time the
		// software writes the register DAC_SHSR2 , It is cleared by hardware when the write operation
		// of DAC_SHSR2 is complete. ( It takes about 3 LSI periods of synchronization).
		//   0: There is no write operation of DAC_SHSR2 ongoing : DAC_SHSR2 can be written
		//   1: There is a write operation of DAC_SHSR2 ongoing : DAC_SHSR2 cannot be written
		while ( (DAC->SR & DAC_SR_BWST2) != 0);
		
		// DAC channel2 12-bit right aligned data holding register;
		DAC->DHR12R2 = (i++) % 4095;
		
		// DAC software trigger register (DAC_SWTRGR)
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2; 
		
		delay(5);
		GPIOD->ODR &= ~GPIO_ODR_ODR_0;  // Set PD 0 as low
		// The duty ratio of the signal on PD 0 represents CPU utilization
	}
}

