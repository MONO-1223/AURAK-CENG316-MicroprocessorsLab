#ifndef __STM32L476G_DISCOVERY_DAC_H
#define __STM32L476G_DISCOVERY_DAC_H

#include "stm32l476xx.h"

#define  DAC_SAMPLE_SIZE   ADC_SAMPLE_SIZE

void DAC_Init(void);
void DAC_Pin_Configuration(void);
void DAC_Configuration(void);
void DAC_Calibration_Channel(uint32_t channel);

#endif /* __STM32L476G_DISCOVERY_DAC_H */

