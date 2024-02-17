#include "lcd.h"
#include "stm32l476xx.h"
#include <stdint.h> // defines standard integer types like uint8_t, int16_t, etc. 
// By including this file, the code ensures that these integer types are available for use.

// Including these header files at the beginning is a common practice in embedded systems programming. 
// It allows the compiler to understand the hardware-specific details and use standardized integer types.

// There are multiple functions defined in this file - above each is a comment whethere we should focus on modifying it or keep it unchanged

const uint16_t CapLetterMap[26] = // Constant table for characters A -> Z mapping them to corresponding LCD segment patterns in hexa.
{
        /* A      B      C      D      E      F      G      H      I  */
        0xFE00,0x6714,0x1d00,0x4714,0x9d00,0x9c00,0x3f00,0xfa00,0x0014,
        /* J      K      L      M      N      O      P      Q      R  */
        0x5300,0x9841,0x1900,0x5a48,0x5a09,0x5f00,0xFC00,0x5F01,0xFC01,
        /* S      T      U      V      W      X      Y      Z  */
        0xAF00,0x0414,0x5b00,0x18c0,0x5a81,0x00c9,0x0058,0x05c0
};


const uint16_t NumberMap[10] = // Constant table for number 0 -> 9 mapping them to corresponding LCD segment patterns in hexa.
{
        /* 0      1      2      3      4      5      6      7      8      9  */
        0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
};



// Defining several macros and constant codes used for controlling and displaying various symbols and characters on an LCD. 

// These macros control the status of LCD bars. The bars are represented using bits in the t_bar array.
uint8_t t_bar[2] = {0x00,0x00};
#define BAR0_ON  t_bar[1] |= 8
#define BAR0_OFF t_bar[1] &= ~8
#define BAR1_ON  t_bar[0] |= 8
#define BAR1_OFF t_bar[0] &= ~8
#define BAR2_ON  t_bar[1] |= 2
#define BAR2_OFF t_bar[1] &= ~2
#define BAR3_ON  t_bar[0] |= 2 
#define BAR3_OFF t_bar[0] &= ~2 

// Hexadecimal values representing the activation status of different segments on the LCD for each symbol.
#define DOT                   ((uint16_t) 0x8000 ) // add decimal point in string
#define DOUBLE_DOT            ((uint16_t) 0x4000) // add decimal point in string
#define C_OPENPARMAP          ((uint16_t) 0x0028) // add'(' character
#define C_CLOSEPARMAP         ((uint16_t) 0x0011) // add ')' character
#define C_DMAP                ((uint16_t) 0xf300) // add 'd' character
#define C_MMAP                ((uint16_t) 0xb210) // add 'm' character
#define C_NMAP                ((uint16_t) 0x2210) // add 'n' character
#define C_UMAP                ((uint16_t) 0x6084) // add 'µ' character
#define C_STAR                ((uint16_t) 0xA0DD) // add '*' character
#define C_MINUS               ((uint16_t) 0xA000) // add '-' character
#define C_PLUS                ((uint16_t) 0xA014) // add '+' character
#define C_SLATCH              ((uint16_t) 0x00c0) // add '/' character
#define C_PERCENT_1           ((uint16_t) 0xec00) // add '°' character
#define C_PERCENT_2           ((uint16_t) 0xb300) // add 'o' character
#define C_FULL                ((uint16_t) 0xffdd)


void LCD_Initialization(void) // a higher-level initialization routine
// calling other functions to set up the pins, clock, and specific configuration for the LCD. 
{
	LCD_PIN_Init();
	LCD_Clock_Init();	
	LCD_Configure();
	LCD_Clear();
}

// Focus on modifying - Reference section 1 tables in the report
void LCD_PIN_Init(void) // initializes the GPIO pins that are used to interface with an LCD
{
	// Enables the clock for the GPIO ports A, B, C, and D.
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIODEN; 
	
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// The << operator is the left shift operator in C. 
	// It shifts the bits on the left of the operand to the left by the number of positions specified to the right of the operand.
	// For example, 3U<<(2*6) means shifting the UNSIGNED number 3 twelve (2*6) positions to the left.
	// The reason for multiplying the pin numbers by 2 is because each pin mode in the MODER register occupies 2 bits.

	// 0U in binary is 00
	// 1U in binary is 01
	// 2U in binary is 10
	// 3U in binary is 11
	// Left-shifting 0000 0000 0000 0000 0000 0000 0000 0011 by 12 positions results in 0000 0000 0000 0000 0011 0000 0000 0000
	// ~(3U<<(2*6)) is the bitwise NOT operation, inverting all bits, so 0000 0000 0000 0000 0011 0000 0000 0000 becomes 1111 1111 1111 1111 1100 1111 1111 1111.
	// Proof: https://i.imgur.com/dIyxIjV.png
	// GPIOA->MODER is the value currently stored in the MODER register for GPIOA.
	// GPIOA->MODER &= ~(3U<<(2*6)) performs a bitwise AND operation between the current value in GPIOA->MODER and the inverted bitmask.
	// This clears (sets to 0) the bits at positions 12 and 13, leaving the rest of the bits unchanged. 
	// Because anything ANDed with a 0 becomes 0 and andything ANDed with 1 depends on the value of the other number - so if it was 0 the result is 0 and if it was 1 the result is 1 so it stays the same.
	// The bitwise AND operation with the inverted bitmask is used to selectively clear specific bits in the MODER register while leaving others unchanged.	
	// If we were to use 0U instead of 3U, the resulting bitmask would be all zeros, and performing an AND operation with it would clear all the bits corresponding to the specified pins, effectively setting them to 0.
	// So, using 3U in this context ensures that only the bits you want to clear are set to 0, while the other bits remain unchanged.
		
		
	// Configuring port A pins PA6, PA7, PA8, PA9, PA10, and PA15 for alternative function 11 (0x0B)
	/*
	GPIOA->MODER   &= ~(3U<<(2*6) | 3U<<(2*7) | 3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10) | 3U<<(2*15)); // This is done to ensure that the bits for these pins are reset to a known state before any subsequent configurations are applied.
	// The reason for clearing only the bits corresponding to the specified pins and not touching the rest is to avoid unintentional changes to the configuration of other pins. 
	// If you were to reset all the bits in the MODER register, it might affect the configuration of pins that you don't intend to modify, leading to unexpected behavior in your application.
	// The bitwise OR operation 3U<<(2*6) | 3U<<(2*7) is used to combine the individual bit patterns for each pin.
	GPIOA->MODER   |=   2U<<(2*6) | 2U<<(2*7) | 2U<<(2*8) | 2U<<(2*9) | 2U<<(2*10) | 2U<<(2*15) ; // sets the mode for the same pins to alternate function mode 
	
	
	GPIOA->AFR[0]  |= 0xBB000000; // sets the alternate function for pins 6, 7, 8, and 9 in AFR[0] register.
	GPIOA->AFR[1]  |= 0xB0000BBB; // sets the alternate function for pins 10 and 15 in AFR[1] register.
	// The reason the alternate function register (AFR) lines are structured differently is due to the nature of the AFR register on some ARM Cortex-M microcontrollers.
	// In many ARM Cortex-M microcontrollers, the AFR register is divided into two parts: AFR[0] and AFR[1]. Each part corresponds to a different group of pins. 
	// In this specific code, pins 6, 7, 8, and 9 belong to AFR[0], and pins 10 and 15 belong to AFR[1]. Therefore, the configuration for these two groups of pins is handled separately.
	// The hexadecimal values used in these lines represent the specific alternate function configuration for each group of pins.
	// The bitwise OR operation is used to set specific bits in the AFR[0] and AFR[1] registers without affecting the other bits.
	// The existing values in the AFR registers are not cleared before this operation.
	// The reason for not explicitly resetting the AFR values is that each pin's alternate function is typically represented by a specific bit pattern within the AFR register. 
	// If you were to clear the entire AFR register, it might inadvertently affect the configurations of other pins that share the same AFR register.
	
	GPIOA->OSPEEDR &= ~(3U<<(2*6) | 3U<<(2*7) | 3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10) | 3U<<(2*15)); // clears the output speed bits for the specified pins in the OSPEEDR register.
	GPIOA->OSPEEDR |=   2U<<(2*6) | 2U<<(2*7) | 2U<<(2*8) | 2U<<(2*9) | 2U<<(2*10) | 2U<<(2*15) ; // sets the output speed for the specified pins in the OSPEEDR register to high speed.
	
	GPIOA->PUPDR   &= ~(3U<<(2*6) | 3U<<(2*7) | 3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10) | 3U<<(2*15)); // clears the pull-up/pull-down bits for the specified pins in the PUPDR register.
  */
	
	// Note that this ^ is not the only way to configure the pins of port A - we can also use the calculated hexadecimal values of the prelab as an equivalent compact approach
	GPIOA->MODER   &= ~0xC03FF000; // using the inverted hex bitmask to clear values of the specific pins
	GPIOA->MODER   |= 0x802AA000; // using the hex value to manipulate bits to our desired values
	GPIOA->AFR[0]  |= 0xBB000000; // using the hex value
	GPIOA->AFR[1]  |= 0xB0000BBB; // using the hex value
	GPIOA->OSPEEDR &= ~0xC03FF000; // using the inverted hex bitmask to clear values of the specific pins (same as MODER because we're affecting the same pins)
	GPIOA->OSPEEDR |= 0x802AA000; // using the hex value to manipulate bits to our desired values (same as MODER because we're affecting the same pins)
	GPIOA->PUPDR   &= ~0xC03FF000; // using the inverted hex bitmask to clear values of the specific pins (same as MODER because we're affecting the same pins)
	
	// Configuring port B pins PB0, PB1, PB4, PB5, PB9, PB12, PB13, PB14, and PB15 as alternative function 11 (0x0B)
	/*
	GPIOB->MODER   &= ~(3U<<(2*0) | 3U<<(2*1) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*9) | 3U<<(2*12)| 3U<<(2*13)| 3U<<(2*14)| 3U<<(2*15));
	GPIOB->MODER   |=   2U<<(2*0) | 2U<<(2*1) | 2U<<(2*4) | 2U<<(2*5) | 2U<<(2*9) | 2U<<(2*12)| 2U<<(2*13)| 2U<<(2*14)| 2U<<(2*15);
	GPIOB->AFR[0]  |= 0x00BB00BB;  
	GPIOB->AFR[1]  |= 0xBBBB00B0; 
	GPIOB->OSPEEDR &= ~(3U<<(2*0) | 3U<<(2*1) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*9) | 3U<<(2*12)| 3U<<(2*13)| 3U<<(2*14)| 3U<<(2*15));
	GPIOB->OSPEEDR |=   2U<<(2*0) | 2U<<(2*1) | 2U<<(2*4) | 2U<<(2*5) | 2U<<(2*9) | 2U<<(2*12)| 2U<<(2*13)| 2U<<(2*14)| 2U<<(2*15);
	GPIOB->PUPDR   &= ~(3U<<(2*0) | 3U<<(2*1) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*9) | 3U<<(2*12)| 3U<<(2*13)| 3U<<(2*14)| 3U<<(2*15));
	*/
	GPIOB->MODER   &= ~0xFF0C0F0F;
	GPIOB->MODER   |= 0xAA080A0A;
	GPIOB->AFR[0]  |= 0x00BB00BB;  
	GPIOB->AFR[1]  |= 0xBBBB00B0; 
	GPIOB->OSPEEDR &= ~0xFF0C0F0F;
	GPIOB->OSPEEDR |= 0xAA080A0A;
	GPIOB->PUPDR   &= ~0xFF0C0F0F;
	
  // Configuring port C pins PC3, PC4, PC5, PC6, PC7, and PC8 as alternative function 11 (0x0B) - Note: PC3 = V_LCD
	/*
	GPIOC->MODER   &= ~(3U<<(2*3) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*6)  | 3U<<(2*7) | 3U<<(2*8));
	GPIOC->MODER   |=   2U<<(2*3) | 2U<<(2*4) | 2U<<(2*5) | 2U<<(2*6)  | 2U<<(2*7) | 2U<<(2*8);
	GPIOC->AFR[0]  |= 0xBBBBB000;   
	GPIOC->AFR[1]  |= 0x0000000B; 
	GPIOC->OSPEEDR &= ~(3U<<(2*3) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*6)  | 3U<<(2*7) | 3U<<(2*8));
	GPIOC->OSPEEDR |=   2U<<(2*3) | 2U<<(2*4) | 2U<<(2*5) | 2U<<(2*6)  | 2U<<(2*7) | 2U<<(2*8);
	GPIOC->PUPDR   &= ~(3U<<(2*3) | 3U<<(2*4) | 3U<<(2*5) | 3U<<(2*6)  | 3U<<(2*7) | 3U<<(2*8));
	*/
	GPIOC->MODER   &= ~0x0003FFC0;
	GPIOC->MODER   |= 0x0002AA80;
	GPIOC->AFR[0]  |= 0xBBBBB000;  
	GPIOC->AFR[1]  |= 0x0000000B; 
	GPIOC->OSPEEDR &= ~0x0003FFC0;
	GPIOC->OSPEEDR |= 0x0002AA80;
	GPIOC->PUPDR   &= ~0x0003FFC0;
	
	// Configuring port D pins PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15 as alternative function 11 (0x0B)
	/*
	GPIOD->MODER   &= ~(3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10)  | 3U<<(2*11) | 3U<<(2*12) | 3U<<(2*13)| 3U<<(2*14) | 3U<<(2*15));
	GPIOD->MODER   |= 2U<<(2*8) | 2U<<(2*9) | 2U<<(2*10)  | 2U<<(2*11) | 2U<<(2*12) | 2U<<(2*13)| 2U<<(2*14) | 2U<<(2*15);
	GPIOD->AFR[1]  |= 0xBBBBBBBB; 
	GPIOD->OSPEEDR &= ~(3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10)  | 3U<<(2*11) | 3U<<(2*12) | 3U<<(2*13)| 3U<<(2*14) | 3U<<(2*15));
	GPIOD->OSPEEDR |= 2U<<(2*8) | 2U<<(2*9) | 2U<<(2*10)  | 2U<<(2*11) | 2U<<(2*12) | 2U<<(2*13)| 2U<<(2*14) | 2U<<(2*15);	
	GPIOD->PUPDR   &= ~(3U<<(2*8) | 3U<<(2*9) | 3U<<(2*10)  | 3U<<(2*11) | 3U<<(2*12) | 3U<<(2*13)| 3U<<(2*14) | 3U<<(2*15));
	*/

	GPIOD->MODER   &= ~0xFFFF0000; // Clear bits for pins 8 to 15
	GPIOD->MODER   |= 0xAAAA0000;  // Set alternative function mode for pins 8 to 15
	GPIOD->AFR[0]  |= 0x00000000;  // Set alternate function 11 for pins 0 to 7
	GPIOD->AFR[1]  |= 0xBBBBBBBB;  // Set alternate function 11 for pins 8 to 15
	GPIOD->OSPEEDR &= 0xFFFF0000;  // Clear output speed bits for pins 8 to 15
	GPIOD->OSPEEDR |= 0xAAAA0000;  // Set high-speed mode for pins 8 to 15
	GPIOD->PUPDR   &= 0xFFFF0000;  // Clear pull-up/pull-down bits for pins 8 to 15
}

// Focus on modifying - Reference section 2 tables in the report
void LCD_DisplayName(void) // The long method
{
	while (LCD->SR&LCD_SR_UDR); // Wait for Update Display Request Bit
	
	// Comment and uncomment the RAM[] config sections based on what word you want displayed
	// You cannot have both commented or uncommented at the same time.
	
	// This is RAM[] configuration to display MONO
	LCD->RAM[0] = 0x90409050;
	LCD->RAM[1] = 0x00000001;
	LCD->RAM[2] = 0xE0807068;
	LCD->RAM[3] =  0x00000003;
	LCD->RAM[4] =  0x00400000;
	LCD->RAM[5] =  0x00000000;
	LCD->RAM[6] = 0x0000C008;
	LCD->RAM[7] = 0x00000000;
	
	
	/* // This is RAM[] configuration to display AURAK
	LCD->RAM[0] = 0xF0C0D058;
	LCD->RAM[1] = 0x0000000F;
	LCD->RAM[2] = 0x50C06068;
	LCD->RAM[3] =  0x00000007;
	LCD->RAM[4] =  0x01000000;
	LCD->RAM[5] =  0x00000000;
	LCD->RAM[6] = 0x00008000;
	LCD->RAM[7] = 0x00000008;
	*/

	LCD->SR |= LCD_SR_UDR; //request update display
	
	while ((LCD->SR&LCD_SR_UDD) == 0); //wait for update display done
}

// Unmodified for lab2 purposes
void LCD_Clock_Init(void) //  Needed to initialize the LCD clocks.
{
	// Enable write access to Backup domain
	if ( (RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0)
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;	// Power interface clock enable
	(void) RCC->APB1ENR1;  // Delay after an RCC peripheral clock enabling
	
	// Select LSE as RTC clock soucre 
	if ( (PWR->CR1 & PWR_CR1_DBP) == 0) 
	{
		PWR->CR1  |= PWR_CR1_DBP;				  			// Enable write access to Backup domain
		while((PWR->CR1 & PWR_CR1_DBP) == 0);  	// Wait for Backup domain Write protection disable
	}
	
	// Reset LSEON and LSEBYP bits before configuring the LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

	// RTC Clock selection can be changed only if the Backup Domain is reset
	RCC->BDCR |=  RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	
	// Note from STM32L4 Reference Manual: 	
  // RTC/LCD Clock:  (1) LSE is in the Backup domain. (2) HSE and LSI are not.	
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
	{  // Wait until LSE clock ready
		RCC->BDCR |= RCC_BDCR_LSEON;
	}
	
	// Select LSE as RTC clock source
	// BDCR = Backup Domain Control Register 
	RCC->BDCR	&= ~RCC_BDCR_RTCSEL;	  // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE
	RCC->BDCR	|= RCC_BDCR_RTCSEL_0;   // Select LSE as RTC clock	
	RCC->BDCR |= RCC_BDCR_RTCEN;      // Enable RTC clock to enable accesses to BDCR
	
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN;	// Power interface clock disable
	
	// Wait for the external capacitor Cext which is connected to the VLCD pin is charged (approximately 2ms for Cext=1uF) 
	
	// Enable LCD peripheral Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_LCDEN;
	
	// Enable SYSCFG 
	// RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}
	
// Focus on modifying - Reference section 3 tables in the report
void LCD_Configure(void)
	// Initializes the LCD controller with specific configurations for clock, blink mode, contrast, duty cycle, bias, and voltage source. 
	// After the initialization, the LCD controller is enabled, and the code waits for the controller to be ready to provide the correct voltage. Finally, it enables the display request to start the LCD operation.
{
	LCD->CR &= ~LCD_CR_LCDEN;	 // Disable LCD
	
	// LCD frame control register (FCR)
	// Set clock divider 
	LCD->FCR |=  0xF<<18;       // DIV[3:0] = 1111, ck_div = ck_ps/31
		
	// Set CLKPS = LCDCLK
	LCD->FCR &= ~LCD_FCR_PS;    // 0000: ck_ps = LCDCLK
	
	// Blink mode selection
  LCD->FCR &= ~LCD_FCR_BLINK; // 00: Blink disabled
															// 01: Blink enabled on SEG[0], COM[0] (1 pixel)
															// 10: Blink enabled on SEG[0], all COMs (up to 8 pixels depending on the programmed duty)
															// 11: Blink enabled on all SEGs and all COMs (all pixels)
	// Blink Frequency
	LCD->FCR &= ~LCD_FCR_BLINKF; 
	LCD->FCR |= LCD_FCR_BLINKF_1;  // 010: fLCD/32	
    
	// Set Pulse ON duration
	// Use high drive internal booster to provide larger drive current
	// Set the duration that the low-resister voltage divider is used
	LCD->FCR |= 0x7 << 4;     // PON[2:0] = 0x111
	
	// Contrast Control: specify one of the VLCD maximum voltages
	LCD->FCR &= ~LCD_FCR_CC;  // CC[2:0] bits (Contrast Control)
	LCD->FCR |= 0x7<<10; 			// 111: VLCD7 as per the lab instructions

	// Pulse ON duration: A short pulse will lead to lower power consumption, but displays with high internal resistance may need a longer pulse to achieve satisfactory contrast. 
	LCD->FCR &= ~LCD_FCR_PON; // PON[2:0] bits (Pulse ON Duration)
	LCD->FCR |= 0x4<<4; // 100: 4/ck_ps
    
	// Waits until the LCD FCR register is synchronized in the LCDCLK domain
	// This bit is set by hardware each time the LCD_FCR register is updated in the LCDCLK domain.
	// It is cleared by hardware when writing to the LCD_FCR register.
	while ((LCD->SR & LCD_SR_FCRSR) == 0);  // Loop until FCRSF flag is set
	
	// Select 1/4 duty
	LCD->CR &= ~(LCD_CR_DUTY);  // Clear duty
	LCD->CR |= 0x3<<2; 		      // DUTY[2:0]: 000=Static; 001=1/2; 010=1/3; 011=1/4; 100=1/8	

	// Select 1/3 bias
	LCD->CR &= ~(LCD_CR_BIAS);  // Clear bias
	LCD->CR |= 0x2<<5; 		      // BIAS[1:0]: 00=1/4; 01=1/2; 10=1/3	
	
	// MUX_SEG disabled
	// 0: SEG pin multiplexing disabled
  // 1: SEG[31:28] are multiplexed with SEG[43:40]
	LCD->CR &= ~LCD_CR_MUX_SEG; // Disable Mux Segment; 
	
	/* LCD control register */
	// VSEL: Voltage source selection
	// When the LCD is based on the internal step-up converter, the VLCD pin should be connected to a capacitor (see the product datasheet for further information).
	LCD->CR &= ~LCD_CR_VSEL;    // 0 = internal source, 1 = external source (VLCD pin)	
	
	// LCD controller enable
	// The VSEL, MUX_SEG, BIAS, DUTY and BUFEN bits are write-protected when the LCD is enabled (ENS bit in LCD_SR to 1).
	// When the LCD is disabled all COM and SEG pins are driven to VSS.
	LCD->CR |= LCD_CR_LCDEN;	 // Enable LCD
	// Loop until LCD controller is enabled
	while ((LCD->SR & LCD_SR_ENS) == 0);  // ENS is set by hardware automatically
		
	// Wait until the LCD Booster is ready
	while ((LCD->SR & LCD_SR_RDY) == 0);  // Loop until step-up converter is ready to provide the correct voltage.
	
  // Enable the display request
  LCD->SR |= LCD_SR_UDR; 	
}


void LCD_Clear(void)
{
  uint8_t counter = 0;

  // Wait until LCD ready 
	while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
  
  for (counter = 0; counter <= 15; counter++) {
    LCD->RAM[counter] = 0;
  }

  // Update the LCD display 
	LCD->SR |= LCD_SR_UDR; 
}


void LCD_DisplayString(uint8_t* ptr) // The shortcut function
{
	uint8_t position = 0;
		uint8_t colonOn = 0;
	uint8_t pointOn = 0;

  // Send the string character by character on lCD 
  while ( (*ptr != 0) && (position < 6)) {
		
		if (*(ptr + 1) == ':')
				colonOn = 1;
		else
				colonOn = 0;
		
		if (* (ptr + 1) == '.')
				pointOn = 1;
		else
				pointOn = 0;
		
    LCD_WriteChar( ptr, pointOn, colonOn, position); // Display one character on LCD 
		
    ptr++;
		if (colonOn) ptr++;
		if (pointOn) ptr++;
		position++;
  }	
}


void LCD_bar(void) 
{
// TO wait LCD Ready *
while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
// Bar 0: COM3, LCD_SEG11 -> MCU_LCD_SEG8
// Bar 1: COM2, LCD_SEG11 -> MCU_LCD_SEG8
// Bar 2: COM3, LCD_SEG9 -> MCU_LCD_SEG25
// Bar 3: COM2, LCD_SEG9 -> MCU_LCD_SEG25
LCD->RAM[4] &= ~(1U << 8 | 1U << 25);
LCD->RAM[6] &= ~(1U << 8 | 1U << 25);
/* bar1 bar3 */
	
if (BAR0_ON)
    LCD->RAM[6] |= (1U << 8);
if (BAR1_ON)
    LCD->RAM[4] |= (1U << 8);
if (BAR2_ON)
    LCD->RAM[6] |= (1U << 25);
if (BAR1_ON)
    LCD->RAM[4] |= (1U << 25);

LCD->SR |= LCD_SR_UDR;
}

// Unmodified for lab2 purposes
static void LCD_Conv_Char_Seg(uint8_t* c, bool point, bool colon, uint8_t* digit)
// this function takes a character, determines its segment representation based on predefined mappings, and considers options for displaying a decimal point and colon. 
// The resulting segment representation is stored in the digit array. This kind of conversion is common in applications where characters need to be displayed on segmented LCDs
{
  uint16_t ch = 0 ;
  uint8_t loop = 0, index = 0;
  
  switch (*c) // checks the value of the character pointed to by c and sets the variable ch accordingly. Various characters, including digits, special characters, and uppercase and lowercase letters, are considered.
    // The code uses predefined constants (like C_STAR, C_OPENPARMAP, etc.) or arrays (NumberMap, CapLetterMap) to map each character to its corresponding segment representation.
	{
    case ' ' :
      ch = 0x00;
      break;

    case '*':
      ch = C_STAR;
      break;

    case '(' :
      ch = C_OPENPARMAP;
      break;

    case ')' :
      ch = C_CLOSEPARMAP;
      break;
      
    case 'd' :
      ch = C_DMAP;
      break;
    
    case 'm' :
      ch = C_MMAP;
      break;
    
    case 'n' :
      ch = C_NMAP;
      break;

    case 'µ' :
      ch = C_UMAP;
      break;

    case '-' :
      ch = C_MINUS;
      break;

    case '+' :
      ch = C_PLUS;
      break;

    case '/' :
      ch = C_SLATCH;
      break;  
      
    case '°' :
      ch = C_PERCENT_1;
      break;  
		
    case '%' :
      ch = C_PERCENT_2; 
      break;
		
    case 255 :
      ch = C_FULL;
      break ;
    
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':			
      ch = NumberMap[*c-0x30];		
      break;
          
    default:
      /* The character c is one letter in upper case*/
      if ( (*c < 0x5b) && (*c > 0x40) )
      {
        ch = CapLetterMap[*c-'A'];
      }
      /* The character c is one letter in lower case*/
      if ( (*c <0x7b) && ( *c> 0x60) )
      {
        ch = CapLetterMap[*c-'a'];
      }
      break;
  }
       
  /* Set the digital point can be displayed if the point is on */
  if (point)
  {
    ch |= 0x0002;
  }

  /* Set the "COL" segment in the character that can be displayed if the colon is on */
  if (colon)
  {
    ch |= 0x0020;
  }		

	for (loop = 12,index=0 ;index < 4; loop -= 4,index++)
  {
    digit[index] = (ch >> loop) & 0x0f; /*To isolate the less significant digit */
  }
	
}

// Unmodified for lab2 purposes
void LCD_WriteChar(uint8_t* ch, bool point, bool colon, uint8_t position)
	// This function takes four parameters:
	// ch: A pointer to a uint8_t, representing the character to be displayed.
	// point: A boolean indicating whether a decimal point should be displayed.
	// colon: A boolean indicating whether a colon should be displayed.
	// position: An integer indicating the position (digit) on the LCD where the character should be displayed.
{
  uint8_t digit[4];     /* Digit frame buffer */
   
  // Convert displayed character in segment in array digit 
  LCD_Conv_Char_Seg(ch, point, colon, digit); // function call

  // TO wait LCD Ready *
  while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
  
  switch (position) // determine the position on the LCD where the character should be displayed then update the LCD RAM based on the segments of the digit array
	{
		
    /* Position 1 on LCD (digit1)*/
    case 0:
	
			LCD->RAM[0] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
      LCD->RAM[2] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
      LCD->RAM[4] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
      LCD->RAM[6] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
			/* 1G 1B 1M 1E */
      LCD->RAM[0] |= ((digit[0] & 0x1) << 4) | (((digit[0] & 0x2) >> 1) << 23) | (((digit[0] & 0x4) >> 2) << 22) | (((digit[0] & 0x8) >> 3) << 3);
      /* 1F 1A 1C 1D  */
      LCD->RAM[2] |= ((digit[1] & 0x1) << 4) | (((digit[1] & 0x2) >> 1) << 23) | (((digit[1] & 0x4) >> 2) << 22) | (((digit[1] & 0x8) >> 3) << 3);
      /* 1Q 1K 1Col 1P  */
      LCD->RAM[4] |= ((digit[2] & 0x1) << 4) | (((digit[2] & 0x2) >> 1) << 23) | (((digit[2] & 0x4) >> 2) << 22) | (((digit[2] & 0x8) >> 3) << 3);
      /* 1H 1J 1DP 1N  */
      LCD->RAM[6] |= ((digit[3] & 0x1) << 4) | (((digit[3] & 0x2) >> 1) << 23) | (((digit[3] & 0x4) >> 2) << 22) | (((digit[3] & 0x8) >> 3) << 3);

			break;

    /* Position 2 on LCD (digit2)*/
    case 1:
			
			LCD->RAM[0] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
      LCD->RAM[2] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
      LCD->RAM[4] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
      LCD->RAM[6] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
			/* 2G 2B 2M 2E */
      LCD->RAM[0] |= ((digit[0] & 0x1) << 6) | (((digit[0] & 0x2) >> 1) << 13) | (((digit[0] & 0x4) >> 2) << 12) | (((digit[0] & 0x8) >> 3) << 5);
      /* 2F 2A 2C 2D  */
      LCD->RAM[2] |= ((digit[1] & 0x1) << 6) | (((digit[1] & 0x2) >> 1) << 13) | (((digit[1] & 0x4) >> 2) << 12) | (((digit[1] & 0x8) >> 3) << 5);
      /* 2Q 2K 2Col 2P  */
      LCD->RAM[4] |= ((digit[2] & 0x1) << 6) | (((digit[2] & 0x2) >> 1) << 13) | (((digit[2] & 0x4) >> 2) << 12) | (((digit[2] & 0x8) >> 3) << 5);
      /* 2H 2J 2DP 2N  */
      LCD->RAM[6] |= ((digit[3] & 0x1) << 6) | (((digit[3] & 0x2) >> 1) << 13) | (((digit[3] & 0x4) >> 2) << 12) | (((digit[3] & 0x8) >> 3) << 5);

			break;
    
    /* Position 3 on LCD (digit3)*/
    case 2:
			
			LCD->RAM[0] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
      LCD->RAM[2] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
      LCD->RAM[4] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
      LCD->RAM[6] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
			/* 3G 3B 3M 3E */
      LCD->RAM[0] |= ((digit[0] & 0x1) << 15) | (((digit[0] & 0x2) >> 1) << 29) | (((digit[0] & 0x4) >> 2) << 28) | (((digit[0] & 0x8) >> 3) << 14);
      /* 3F 3A 3C 3D */
      LCD->RAM[2] |= ((digit[1] & 0x1) << 15) | (((digit[1] & 0x2) >> 1) << 29) | (((digit[1] & 0x4) >> 2) << 28) | (((digit[1] & 0x8) >> 3) << 14);
      /* 3Q 3K 3Col 3P  */
      LCD->RAM[4] |= ((digit[2] & 0x1) << 15) | (((digit[2] & 0x2) >> 1) << 29) | (((digit[2] & 0x4) >> 2) << 28) | (((digit[2] & 0x8) >> 3) << 14);
      /* 3H 3J 3DP  3N  */
      LCD->RAM[6] |= ((digit[3] & 0x1) << 15) | (((digit[3] & 0x2) >> 1) << 29) | (((digit[3] & 0x4) >> 2) << 28) | (((digit[3] & 0x8) >> 3) << 14);

			break;
    
    /* Position 4 on LCD (digit4)*/
    case 3:
			
			LCD->RAM[0] &= ~( 1U << 31 | 1U << 30);
			LCD->RAM[1] &= ~( 1U << 1 | 1U << 0 );
      LCD->RAM[2] &= ~( 1U << 31 | 1U << 30);
			LCD->RAM[3] &= ~( 1U << 1 | 1U << 0 );
      LCD->RAM[4] &= ~( 1U << 31 | 1U << 30);
			LCD->RAM[5] &= ~( 1U << 1 | 1U << 0 );
      LCD->RAM[6] &= ~( 1U << 31 | 1U << 30);
			LCD->RAM[7] &= ~( 1U << 1 | 1U << 0 );
			/* 4G 4B 4M 4E */
      LCD->RAM[0] |= ((digit[0] & 0x1) << 31) | (((digit[0] & 0x8) >> 3) << 30);
			LCD->RAM[1] |= (((digit[0] & 0x2) >> 1) << 1) | (((digit[0] & 0x4) >> 2) << 0);
      /* 4F 4A 4C 4D */
      LCD->RAM[2] |= ((digit[1] & 0x1) << 31) | (((digit[1] & 0x8) >> 3) << 30);
			LCD->RAM[3] |= (((digit[1] & 0x2) >> 1) << 1) | (((digit[1] & 0x4) >> 2) << 0);
      /* 4Q 4K 4Col 4P  */
      LCD->RAM[4] |= ((digit[2] & 0x1) << 31) | (((digit[2] & 0x8) >> 3) << 30);
			LCD->RAM[5] |= (((digit[2] & 0x2) >> 1) << 1) | (((digit[2] & 0x4) >> 2) << 0);
      /* 4H 4J 4DP  4N  */
      LCD->RAM[6] |= ((digit[3] & 0x1) << 31) | (((digit[3] & 0x8) >> 3) << 30);
			LCD->RAM[7] |= (((digit[3] & 0x2) >> 1) << 1) | (((digit[3] & 0x4) >> 2) << 0);

			break;
    
    /* Position 5 on LCD (digit5)*/
    case 4:
			
			LCD->RAM[0] &= ~( 1U << 25 | 1U << 24);
			LCD->RAM[1] &= ~( 1U << 3 | 1U << 2 );
      LCD->RAM[2] &= ~( 1U << 25 | 1U << 24);
			LCD->RAM[3] &= ~( 1U << 3 | 1U << 2 );
      LCD->RAM[4] &= ~( 1U << 25 | 1U << 24 );
			LCD->RAM[5] &= ~( 1U << 3 | 1U << 2 );
      LCD->RAM[6] &= ~( 1U << 25 | 1U << 24 );
			LCD->RAM[7] &= ~( 1U << 3 | 1U << 2 );
			/* 5G 5B 5M 5E */
      LCD->RAM[0] |= (((digit[0] & 0x2) >> 1) << 25) | (((digit[0] & 0x4) >> 2) << 24);
			LCD->RAM[1] |= ((digit[0] & 0x1) << 3) | (((digit[0] & 0x8) >> 3) << 2);
      /* 5F 5A 5C 5D */
      LCD->RAM[2] |= (((digit[1] & 0x2) >> 1) << 25) | (((digit[1] & 0x4) >> 2) << 24);
			LCD->RAM[3] |= ((digit[1] & 0x1) << 3) | (((digit[1] & 0x8) >> 3) << 2);
      /* 5Q 5K 5Col 5P  */
      LCD->RAM[4] |= (((digit[2] & 0x2) >> 1) << 25) | (((digit[2] & 0x4) >> 2) << 24);
			LCD->RAM[5] |= ((digit[2] & 0x1) << 3) | (((digit[2] & 0x8) >> 3) << 2);
      /* 5H 5J 5DP  5N  */
      LCD->RAM[6] |= (((digit[3] & 0x2) >> 1) << 25) | (((digit[3] & 0x4) >> 2) << 24);
			LCD->RAM[7] |= ((digit[3] & 0x1) << 3) | (((digit[3] & 0x8) >> 3) << 2);

			break;
    
    /* Position 6 on LCD (digit6)*/
    case 5:
			
			LCD->RAM[0] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
      LCD->RAM[2] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
      LCD->RAM[4] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
      LCD->RAM[6] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
			/* 6G 6B 6M 6E */
      LCD->RAM[0] |= ((digit[0] & 0x1) << 17) | (((digit[0] & 0x2) >> 1) << 8) | (((digit[0] & 0x4) >> 2) << 9) | (((digit[0] & 0x8) >> 3) << 26);
      /* 6F 6A 6C 6D */
      LCD->RAM[2] |= ((digit[1] & 0x1) << 17) | (((digit[1] & 0x2) >> 1) << 8) | (((digit[1] & 0x4) >> 2) << 9) | (((digit[1] & 0x8) >> 3) << 26);
      /* 6Q 6K 6Col 6P  */
      LCD->RAM[4] |= ((digit[2] & 0x1) << 17) | (((digit[2] & 0x2) >> 1) << 8) | (((digit[2] & 0x4) >> 2) << 9) | (((digit[2] & 0x8) >> 3) << 26);
      /* 6H 6J 6DP  6N  */
      LCD->RAM[6] |= ((digit[3] & 0x1) << 17) | (((digit[3] & 0x2) >> 1) << 8) | (((digit[3] & 0x4) >> 2) << 9) | (((digit[3] & 0x8) >> 3) << 26);

			break;
    
     default:
      break;
  }

  /* Refresh LCD  bar */
  //LCD_bar();

  // Update the LCD display 
	// Set the Update Display Request.
	//
	// Each time software modifies the LCD_RAM, it must set the UDR bit to transfer the updated
	// data to the second level buffer. The UDR bit stays set until the end of the update and during
	// this time the LCD_RAM is write protected.
	//
	// When the display is enabled, the update is performed only for locations for which
	// commons are active (depending on DUTY). For example if DUTY = 1/2, only the
	// LCD_DISPLAY of COM0 and COM1 will be updated.
	LCD->SR |= LCD_SR_UDR; 								// Update display request. Cleared by hardware
	//while ((LCD->SR & LCD_SR_UDD) == 0);	// Wait Until the LCD display is done
	//LCD->CLR &= ~LCD_CLR_UDDC;            // Clear UDD flag
  
}