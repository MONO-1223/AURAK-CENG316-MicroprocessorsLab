#include "stm32l476xx.h" // provides access to register definitions and other hardware-specific information
#include "lcd.h" // a custom header file containing function prototypes, macros, and other definitions related to an LCD module

void System_Clock_Init(void); 
// Function prototype placed at the beginning of the code so that we can define it after the main()

int main(void) // this is the entry point of the program
{
	// A bunch of function calls in the main body - all functions are located in the LCD.c file
	
	LCD_Initialization();
	LCD_Clear();
	
	// Note that either LCD_DisplayString() or LCD_DisplayName(). 
	// You can't activate both at the same time - they can't compete for the LCD.
	
	//LCD_DisplayString((uint8_t*)"AURAK"); // The shortcut (no need to calculate LCD RAM[] hex values)
	LCD_DisplayName(); // The long method
	
	//LCD_bar(); // Use only if you want to use the bars next to the Alphanumeric display of the LCD
	
	while(1); // to continuously execute the code within the loop
}


void System_Clock_Init(void)
{
	// Enable the Internal High-Speed oscillator (HSI) by setting the HSION bit in the RCC_CR register.
  // The code waits in a loop until the HSI Ready (HSIRDY) flag is set, indicating that the HSI oscillator is ready and stable.
	
	RCC->CR |= RCC_CR_HSION; // Using the bitwise OR assignment operator (|=) to set the HSION bit in the control register. 
	// The existing value in RCC->CR remains unchanged except for the HSION bit, which is set to 1.
	
	while((RCC->CR & RCC_CR_HSIRDY) == 0); // continues to execute as long as the HSI Ready flag is not set
	
	// RCC->CR is accessing the control register (CR) of the Reset and Clock Control (RCC) peripheral. 
	// The RCC is responsible for configuring the system clock and other clock-related settings on STM32 microcontrollers.
	
	// RCC_CR_HSIRDY is a constant or macro representing the High-Speed Internal oscillator Ready flag. 
	
	// If the flag is set, the result is non-zero; otherwise, it's zero.
}