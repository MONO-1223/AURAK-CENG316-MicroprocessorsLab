#include "stm32l476xx.h"

#define RED_LED_PIN 2
#define GREEN_LED_PIN 8

// Comment and uncomment either of the red or green LED sections as needed - note that they cant be both on or off

int main(void){
		
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
  
	/*
	// Button Lights Red LED
		
  // Enable the clock to GPIO Port B for red LED
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   
		
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->MODER &= ~(3UL<<(2*RED_LED_PIN));  
	GPIOB->MODER |=   1UL<<(2*RED_LED_PIN);      // Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOB->OSPEEDR &= ~(3<<(2*RED_LED_PIN));
	GPIOB->OSPEEDR |=   2<<(2*RED_LED_PIN);  // Fast speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOB->OTYPER &= ~(1<<RED_LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOB->PUPDR  &= ~(3<<(2*RED_LED_PIN));  // No pull-up, no pull-down
	
  // Light up the LED	
	GPIOB->ODR |= 1 << RED_LED_PIN;
	
	*/
		
	
	// Button Lights Green LED
	
	// Enable the clock to GPIO Port E for green LED
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN; 
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOE->MODER &= ~(3UL<<(2*GREEN_LED_PIN));  
	GPIOE->MODER |=   1UL<<(2*GREEN_LED_PIN);      // Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOE->OSPEEDR &= ~(3<<(2*GREEN_LED_PIN));
	GPIOE->OSPEEDR |=   2<<(2*GREEN_LED_PIN);  // Fast speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOE->OTYPER &= ~(1<<GREEN_LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOE->PUPDR  &= ~(3<<(2*GREEN_LED_PIN));  // No pull-up, no pull-down
	
  // Light up the LED	
	GPIOE->ODR |= 1 << GREEN_LED_PIN;
	
	
	
  // Dead loop & program hangs here
	while(1);
}
