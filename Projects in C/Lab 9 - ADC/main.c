#include "stm32l476xx.h"    // Include STM32L476xx header file
#include "LCD.h"            // Include LCD header file
#include "ADC.h"            // Include ADC header file
#include "LED.h"            // Include LED header file
#include "SysTimer.h"       // Include SysTimer header file
#include "SysClock.h"       // Include SysClock header file

volatile uint32_t result;   // Declare a volatile variable to store ADC conversion result

int main(void){

    System_Clock_Init();    							// Initialize system clock to 80 MHz
    SysTick_Init();         							// Initialize SysTick timer

    LCD_PIN_Init();         							// Initialize LCD GPIO pins
    LCD_Clock_Init();       							// Initialize LCD clock
    LCD_Configure();        							// Configure LCD settings
    LCD_Clear();            							// Clear LCD screen
    LCD_DisplayString((uint8_t*)"ADC");   // Display "ADC" on LCD screen
    LCD_bar();              							// Draw a bar on LCD screen

    ADC_Init();             							// Initialize ADC

    while(1){
        // while(Microphone_DMA_Done == 0);
        GPIOD->ODR |= GPIO_ODR_ODR_0;  // Set PD 0 pin high

        ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
        while ( (ADC123_COMMON->CSR & ADC_CSR_EOC_MST) == 0); // Wait for ADC conversion to complete
        result = ADC1->DR; // Store ADC conversion result

        GPIOD->ODR &= ~GPIO_ODR_ODR_0; // Set PD 0 pin low 
        // The duty ratio of the signal on PD 0 represents CPU utilization
    }
}
