#include "stm32l476xx.h" // provides access to register definitions and other hardware-specific information
#include "lcd.h" // a custom header file containing function prototypes, macros, and other definitions related to an LCD module

unsigned char FullStep[4];
unsigned char HalfStep[8];

void GPIO_Init(void);
void System_Clock_Init(void);

// Four pins to control the stepper motor: PB 2, PB 3, PB 6, and PB 7
#define PIN1 2
#define PIN2 3
#define PIN3 6
#define PIN4 7

void Full_Stepping_Clockwise(void){
	int i, j;
	uint32_t output;
	for(i = 0; i < 4; i++){	// to iterate over all the indexes of the FullStep[4] array
			for(j = 0; j < 60000; j++) // must optimally be <60k, the more zeroes added, the slower the rotation
		{
			output = GPIOB->ODR; // accessing the address of the output data register
			output &= ~( 1<< PIN1 | 1 << PIN2 | 1 << PIN3 | 1<< PIN4 );	// clearing the values in these pins
			
            switch (i) 
						{
                case 0:
                    output |= 0x00000084;
                    break;
                case 1:
                    output |= 0x00000044;
                    break;
                case 2:
                    output |= 0x00000048;
                    break;
                case 3:
                    output |= 0x00000088;
                    break;
            }
			
			GPIOB->ODR = output; 
		}	
	}
}

void Full_Stepping_CounterClockwise(void){
	int i, j;
	uint32_t output;
	for(i = 0; i < 4; i++){	
			for(j = 0; j < 60000; j++)
		{
			output = GPIOB->ODR;
			output &= ~( 1<< PIN1 | 1 << PIN2 | 1 << PIN3 | 1<< PIN4 );	
			
			            switch (i) 
						{
                case 0:
                    output |= 0x00000088;
                    break;
                case 1:
                    output |= 0x00000048;
                    break;
                case 2:
                    output |= 0x00000044;
                    break;
                case 3:
                    output |= 0x00000084;
                    break;
            }
			
			GPIOB->ODR = output; 
		}
	}		
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Half_Stepping_Clockwise(void){
	int i, j;
	uint32_t output;
	for(i = 0; i < 8; i++){	
			for(j = 0; j < 60000; j++)
		{
			output = GPIOB->ODR;
			output &= ~( 1<< PIN1 | 1 << PIN2 | 1 << PIN3 | 1<< PIN4 );	
			
			      switch (i) 
						{
                case 0:
                    output |= 0x00000080;
                    break;
                case 1:
                    output |= 0x00000084;
                    break;
                case 2:
                    output |= 0x00000004;
                    break;
                case 3:
                    output |= 0x00000044;
                    break;
								case 4:
                    output |= 0x00000040;
                    break;
								case 5:
                    output |= 0x00000048;
                    break;
								case 6:
                    output |= 0x00000008;
                    break;
								case 7:
                    output |= 0x00000088;
                    break;
            }
			
			GPIOB->ODR = output; 
	}	

	}	
}

void Half_Stepping_CounterClockwise(void){
	int i, j;
	uint32_t output;
	for(i = 0; i < 8; i++){	
			for( j = 0; j < 60000; j++)
		{
			output = GPIOB->ODR;
			output &= ~( 1<< PIN1 | 1 << PIN2 | 1 << PIN3 | 1<< PIN4 );	
			
			      switch (i) 
						{
                case 0:
                    output |= 0x00000088;
                    break;
                case 1:
                    output |= 0x00000008;
                    break;
                case 2:
                    output |= 0x00000048;
                    break;
                case 3:
                    output |= 0x00000040;
                    break;
								case 4:
                    output |= 0x00000044;
                    break;
								case 5:
                    output |= 0x00000004;
                    break;
								case 6:
                    output |= 0x00000084;
                    break;
								case 7:
                    output |= 0x00000080;
                    break;
            }
			
			GPIOB->ODR = output; 
	}	
}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int main(void) // this is the entry point of the program
{	
	System_Clock_Init();
	GPIO_Init();
	LCD_Initialization();
	LCD_Clear();
	
	// Enable the clock to GPIO Port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Set PA0, PA1, PA2, PA3, and PA5 as input
	GPIOA->MODER &= ~0xCFF;

	// Configure PA0, PA1, PA2, PA3, PA5 as pull-down
	GPIOA->PUPDR &= ~0xCFF;
	GPIOA->PUPDR |= 0x8AA;
	
		
	// begin main loop
  while(1){
    LCD_Clear();
		
		// While left button is pushed: Use Clockwise Full-Stepping
		while((GPIOA->IDR & 0x2) != 0x00){
		LCD_DisplayString((uint8_t*)"C FS");
		Full_Stepping_Clockwise();
		}
		
		// While right button is pushed: Use Clockwise Half-Stepping
		while((GPIOA->IDR & 0x4) != 0x00){
		LCD_DisplayString((uint8_t*)"C HS");
		Half_Stepping_Clockwise();
		}
		
		// While up button is pushed: Use Counter-Clockwise Full-Stepping
		while((GPIOA->IDR & 0x8) != 0x00){
		LCD_DisplayString((uint8_t*)"CC FS");
		Full_Stepping_CounterClockwise();
		}
		
		// While down button is pushed: Use Counter-Clockwise Half-Stepping
		while((GPIOA->IDR & 0x20) != 0x00){
		LCD_DisplayString((uint8_t*)"CC HS");
		Half_Stepping_CounterClockwise(); 
		}
	
	}
}


void System_Clock_Init(void){
	uint32_t HSITrim;

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
  // must be correctly programmed according to the frequency of the CPU clock
  // (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;
		
	// Enable the Internal High Speed oscillator (HSI
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR	
	HSITrim = 16; // user-programmable trimming value that is added to HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;
	
	RCC->CR    &= ~RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	
	// Select clock source to PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; // 00 = No clock, 01 = MSI, 10 = HSI, 11 = HSE
	
	// Make PLL as 80 MHz
	// f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 16MHz * 20/2 = 160 MHz
	// f(PLL_R) = f(VCO clock) / PLLR = 160MHz/2 = 80MHz
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << 8;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | 1U << 4; // 000: PLLM = 1, 001: PLLM = 2, 010: PLLM = 3, 011: PLLM = 4, 100: PLLM = 5, 101: PLLM = 6, 110: PLLM = 7, 111: PLLM = 8

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;  // 00: PLLR = 2, 01: PLLR = 4, 10: PLLR = 6, 11: PLLR = 8	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable Main PLL PLLCLK output 

	RCC->CR   |= RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	// Select PLL selected as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	
	// Wait until System Clock has been selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// The maximum frequency of the AHB, the APB1 and the APB2 domains is 80 MHz.
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1; SYSCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB high-speed prescaler (APB1) = 1, HCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB high-speed prescaler (APB2) = 1, HCLK not divided
	
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; 
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;	
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable Main PLL PLLSAI3CLK output enable
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // Enable Main PLL PLL48M1CLK output enable
	
	RCC->CR &= ~RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == RCC_CR_PLLSAI1ON );
	
	// Configure and enable PLLSAI1 clock to generate 11.294MHz 
	// 8 MHz * 24 / 17 = 11.294MHz
	// f(VCOSAI1 clock) = f(PLL clock input) *  (PLLSAI1N / PLLM)
	// PLLSAI1CLK: f(PLLSAI1_P) = f(VCOSAI1 clock) / PLLSAI1P
	// PLLUSB2CLK: f(PLLSAI1_Q) = f(VCOSAI1 clock) / PLLSAI1Q
	// PLLADC1CLK: f(PLLSAI1_R) = f(VCOSAI1 clock) / PLLSAI1R
	RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1N;
	RCC->PLLSAI1CFGR |= 24U<<8;
	
	// SAI1PLL division factor for PLLSAI1CLK
	// 0: PLLSAI1P = 7, 1: PLLSAI1P = 17
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1P;
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1PEN;
	
	// SAI1PLL division factor for PLL48M2CLK (48 MHz clock)
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1Q;
	// RCC->PLLSAI1CFGR |= U<<21;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1QEN;
	
	// PLLSAI1 division factor for PLLADC1CLK (ADC clock)
	// 00: PLLSAI1R = 2, 01: PLLSAI1R = 4, 10: PLLSAI1R = 6, 11: PLLSAI1R = 8
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1R; 
	// RCC->PLLSAI1CFGR |= U<<25;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
	
	RCC->CR |= RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == 0);
	
	// SAI1 clock source selection
	// 00: PLLSAI1 "P" clock (PLLSAI1CLK) selected as SAI1 clock
	// 01: PLLSAI2 "P" clock (PLLSAI2CLK) selected as SAI1 clock
	// 10: PLL "P" clock (PLLSAI3CLK) selected as SAI1 clock
	// 11: External input SAI1_EXTCLK selected as SAI1 clock	
	RCC->CCIPR &= ~RCC_CCIPR_SAI1SEL;

	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
}

void GPIO_Init(void){	
	
	/* Enable GPIOs clock */ 	
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOBEN;
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->MODER = ~(3U<<(2*PIN1) | 3U<<(2*PIN2) | 3U<<(2*PIN3) | 3U<<(2*PIN4));  
	GPIOB->MODER |= 1U<<(2*PIN1) | 1U<<(2*PIN2) | 1U<<(2*PIN3) | 1U<<(2*PIN4);      //  Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOB->OSPEEDR &= ~(3U<<(2*PIN1) | 3U<<(2*PIN2) | 3U<<(2*PIN3) | 3U<<(2*PIN4));
	GPIOB->OSPEEDR |=   2U<<(2*PIN1) | 2U<<(2*PIN2) | 2U<<(2*PIN3) | 2U<<(2*PIN4);  // Fast speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOB->OTYPER &= ~(1U<<PIN1 | 1U<<PIN2 | 1U<<PIN3 | 1U<<PIN4);       // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOB->PUPDR  &= ~(3U<<(2*PIN1) | 3U<<(2*PIN2) | 3U<<(2*PIN3) | 3U<<(2*PIN4));  // No pull-up, no pull-down
}