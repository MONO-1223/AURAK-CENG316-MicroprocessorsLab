
(1) Core Frequency 80MHz
(2) DAC, ADC1 (master), ADC2 (Slave) are triggered by TIM4.
(3) Red LED toggles if ADC1 works.
(4) Analog Inputs: 
	* PA1 (ADC12_IN6), PA2 (ADC12_IN7)
	* These pins are not used: PA0 (ADC12_IN5, PA3 (ADC12_IN8) 
	* 0V <=> 0, 3.0V <=> 4095
(5) Analog Outputs: 
	* PA3 (OPAMP1_OUT), PA5 (DAC1_OUT2)
	* PA4 (DAC1_OUT1) is not extended out. Thus we use OPAMP.
	* 0 <=> 0V, 4095 <=> 3.0V 
(6) The duty ratio of PD0 represents CPU utilization.
(7) DMA setting
	* DMA 1 Channel 1  <--->  ADC1 (Master). We do not need DMA for ADC2 (slave).
	* DMA 1 Channel 4  <--->  DFSDM (Microphone)
	* DMA 2 Channel 4  <--->  DAC 1
	* DMA 2 Channel 5  <--->  DAC 
	* DMA 2 Channel 1  <--->  SAI 1 A
