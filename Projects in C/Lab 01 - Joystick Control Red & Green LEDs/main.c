#include "stm32l476xx.h"

int main(void){
	

  
  // Enable the clock to GPIO Ports A, B, and E	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	// Set PB2 as output
  GPIOB->MODER &= ~(0x03<<(2*2)) ;
  GPIOB->MODER |= (1<<4);
		
	// Set PE8 as output
	GPIOE->MODER &= ~(0x03<<(2*8));		 
  GPIOE->MODER |= (1<<16);
		
	// Set PE8 and PB2 output type as push-pull
	GPIOE->OTYPER &= ~(0x100);
	GPIOB->OTYPER &= ~(0x4);
	
	// PE8 and PB2 output type as No pull-up no pull-down
	GPIOE->PUPDR &= ~(0x30000);
	GPIOB->PUPDR &= ~(0x30);
		
	// Set PA0, PA1, PA2, PA3, and PA5 as input
	GPIOA->MODER &= ~0xCFF;

	// Configure PA0, PA1, PA2, PA3, PA5 as pull-down
	GPIOA->PUPDR &= ~0xCFF;
	GPIOA->PUPDR |= 0x8AA;
	
		
	// begin main loop
  while(1){
    
		// Toggle both LEDs when middle button is pushed
		if((GPIOA->IDR & 0x1) != 0x00){
			GPIOE->ODR ^= GPIO_ODR_ODR_8;
			GPIOB->ODR ^= GPIO_ODR_ODR_2;
			while((GPIOA->IDR & 0x1) != 0x00);
		}
		
		// Toggle red LED when left button is pushed
		if((GPIOA->IDR & 0x2) != 0x00){
			GPIOB->ODR ^= GPIO_ODR_ODR_2;
			while((GPIOA->IDR & 0x2) != 0x00);
		}
		
		// Toggle green LED when right button is pushed
		if((GPIOA->IDR & 0x4) != 0x00){
			GPIOE->ODR ^= GPIO_ODR_ODR_8;
			while((GPIOA->IDR & 0x4) != 0x00);
		}
		
		// Set both LEDs to on when up button is pushed
		if((GPIOA->IDR & 0x8) != 0x00){
			GPIOE->ODR |= 0x100;
			GPIOB->ODR |= 0x04;
			while((GPIOA->IDR & 0x8) != 0x00);
		}
		
		// Set both LEDs to off when down button is pushed
		if((GPIOA->IDR & 0x20) != 0x00){
			GPIOE->ODR &= ~0x100;
			GPIOB->ODR &= ~0x04;
			while((GPIOA->IDR & 0x20) != 0x00);
		}
  }
}

