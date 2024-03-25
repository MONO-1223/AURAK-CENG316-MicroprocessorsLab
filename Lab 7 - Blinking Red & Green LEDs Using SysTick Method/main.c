#include "stm32l476xx.h"
#include "LED.h"
#include "SysTimer.h"

volatile uint32_t test;

void System_Clock_Init(void){
	
	RCC->CR |= RCC_CR_MSION; 
	
	// Select MSI as the clock source of System Clock
	RCC->CFGR &= ~RCC_CFGR_SW; 
	
	// Wait until MSI is ready
	while ((RCC->CR & RCC_CR_MSIRDY) == 0); 	
	
	// MSIRANGE can be modified when MSI is OFF (MSION=0) or when MSI is ready (MSIRDY=1). 
	RCC->CR &= ~RCC_CR_MSIRANGE; 
	RCC->CR |= RCC_CR_MSIRANGE_7;  // Select MSI 8 MHz	
 
	// The MSIRGSEL bit in RCC-CR select which MSIRANGE is used. 
	// If MSIRGSEL is 0, the MSIRANGE in RCC_CSR is used to select the MSI clock range.  (This is the default)
	// If MSIRGSEL is 1, the MSIRANGE in RCC_CR is used. 
	RCC->CR |= RCC_CR_MSIRGSEL; 
	
	// Enable MSI and wait until it's ready	
	while ((RCC->CR & RCC_CR_MSIRDY) == 0); 		
}


int main(void){

	test = 1;
	
	System_Clock_Init(); // Set System Clock as 8 MHz
	LED_Init();
	SysTick_Init();
	
	while(1){
		// You may activate either Red_LED_Toggle() or Green_LED_Toggle() or both at the same time.
		// If you want them both on at the same time and off at the same time then call them first then the delay function
		// If you want them to toggle alternately such that when one is on the othet is off, put the delay function between them
		
		Red_LED_Toggle();	
		Green_LED_Toggle();
		delay(1000);
	}
}

