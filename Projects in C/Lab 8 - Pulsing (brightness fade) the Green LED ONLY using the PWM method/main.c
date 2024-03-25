#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"

int brightness = 0;

int main() {
		int i;
		int n = 1;
	
	    // System_Clock_Init(); // Switch System Clock = 80 MHz
	    // We comment the previous line out because we want to use the default system clock = 4 MHz (clock that drives the processor core and its peripherals such as timers)
	    // This is still high though, so we have to divide the frequency to get more accurate results and be able to observe the pulses with our eyes
	
	    // To make the pulses for the green LED
        RCC->AHB2ENR 		|= RCC_AHB2ENR_GPIOEEN;                 // Enable GPIOE clock
	    // Set GPIO Port E Pin 8 I/O direction as Alternative Function 1
		GPIOE->MODER  	&= ~(0x03 << (2*8));   			// Clear bits
		GPIOE->MODER  	|= 0x02 << (2*8);      			// Input(00), Output(01), AlterFunc(10), Analog(11)
		GPIOE->AFR[1] 	|= 0x01 ;                     	// 	AF 1 = TIM1_CH1N
	    //Set I/O output speed value as very high speed
		GPIOE->OSPEEDR  &= ~(0x03<<(2*8)); 				// Speed mask
		GPIOE->OSPEEDR  |=   0x03<<(2*8); 				// Very high speed
	    //Set I/O as no pull-up pull-down 
		GPIOE->PUPDR    &= ~(0x03<<(2*8));    			// No PUPD(00, reset), Pullup(01), Pulldown(10), Reserved (11)

        RCC->APB2ENR 	|= RCC_APB2ENR_TIM1EN;          // Enable TIMER clock

		// Counting direction: 0 = up-counting, 1 = down-counting
		TIM1->CR1 &= ~TIM_CR1_DIR;  //Upcounting is active when the DIR bit in the TIMx_CR1 register is low.

		// This is the line of code we wish to modify - we have to reverse engineer this such that we control the frequency of the pulses based on the period we want
		// The goal is to pick a period of pulsing that is human-eye observable (like 1s or 0.5s or more)
		// 2s period = 7999 PSC (resulting in very slow pulses)
		// 1s period = 3999 PSC (resulting in slow pulses)
		// 0.5s period = 1999 PSC (resulting in medium pulses)
		// 0.1s period = 399 PSC (resulting in very fast but still observable pulses)
		// Method of calculation is shown in the report in detail and can be applied for any other period values
		TIM1->PSC = 3999;                               
		TIM1->ARR = 1000-1;                                       // Auto-reload: Upcouting (0..ARR), Downcouting (ARR..0)
		                                                          //PWM f= counter f/(1+ARR)//PWM period=(999+1)*(1/100KHz)=0.01s
		TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;                           // Clear ouput compare mode bits for channel 1 //(OC1M=110)
		TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;       // OC1M = 110 for PWM Mode 1 output on ch1
		TIM1->CCMR1 |= TIM_CCMR1_OC1PE;                           // Output 1 preload enable

		// Select output polarity: 0 = active high, 1 = active low
		TIM1->CCMR1 &= ~TIM_CCER_CC1NP; // select active high
		
        // Enable output for ch1N//enable complementary output of CH 1 because PE 8 is connected internally to the complementary output
		TIM1->CCER 	|= TIM_CCER_CC1NE;                       
    
        // Main output enable (MOE): 0 = Disable, 1 = Enable
		TIM1->BDTR |= TIM_BDTR_MOE;  

		// In PWM, the duty cycle represents the percentage of time the signal is in the high state (ON) compared to the total period of the signal.
		// For example, a 70% duty cycle means the signal is high (ON) for 70% of the time and low (OFF) for the remaining 30%.
		// Similarly, a 50% duty cycle is 50% ON and 50% OFF.
		TIM1->CCR1  = 500;                //duty cycle 50% // Output Compare Register for channel 1 CCR Reg we store in it a given refrence value
		TIM1->CR1  |= TIM_CR1_CEN;        // Enable counter
			
    while(1) 
	{
		if (((brightness + n) >= 999) || ((brightness + n) <= 0))
			n = -n; // if brightness maximum/maximum change direction
			
      brightness += n;
			TIM1->CCR1 = brightness; 														// set brightness for channel 1
			for(i=0;i<50;i++);  															// delay (busy-waiting approach)
			// a short delay is added between two consecutive updates to CCR to adjust the speed of brightness change. 
	}
}