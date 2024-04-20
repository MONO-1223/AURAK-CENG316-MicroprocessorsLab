	INCLUDE core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s
		
	AREA main, CODE, READONLY
	EXPORT __main
	ENTRY
	
__main PROC
	
	; Recall that in (number1 << number2) number1 is a hex representation of a binary value and number2 is actually just a number by which we shift left a certain number of digits
	
	LDR R0, =RCC_BASE ;This line loads the address of the RCC (Reset and Clock Control) peripheral into register R0. RCC_BASE is typically a symbolic constant representing the base address of the RCC registers.
	LDR R1, [R0, #RCC_AHB2ENR] ;The value from the memory location specified by the sum of the value in register R0 (which holds the address of RCC) and the offset RCC_AHB2ENR is loaded into register R1. RCC_AHB2ENR is an offset representing the address of the AHB2 peripheral clock enable register within the RCC peripheral.
	ORR R1, R1, #RCC_AHB2ENR_GPIOBEN ;Performs a bitwise OR operation between the value in register R1 (which holds the value read from RCC_AHB2ENR) and RCC_AHB2ENR_GPIOBEN. This constant represents the bit position for enabling the clock of GPIO port B. The result is then stored back into register R1.
	STR R1, [R0, #RCC_AHB2ENR] ;The modified value in register R1 is stored back into the memory location specified by the sum of the value in register R0 (address of RCC) and the offset RCC_AHB2ENR, effectively updating the AHB2 peripheral clock enable register within the RCC peripheral with the new value.

	LDR r0, =RCC_BASE
	LDR R1, [R0, #RCC_AHB2ENR]
	ORR R1, R1, #RCC_AHB2ENR_GPIOEEN
	STR R1, [R0, #RCC_AHB2ENR]
	
	LDR R0, =RCC_BASE
	LDR R1, [R0, #RCC_AHB2ENR]
	ORR R1, R1, #RCC_AHB2ENR_GPIOAEN
	STR R1, [R0, #RCC_AHB2ENR]
	

	LDR R0, =GPIOB_BASE ;Load the address of the GPIOB peripheral into register R0. Similar to RCC_BASE, GPIOB_BASE is defined in the microcontroller's header files and points to the base address of the GPIO Port B registers.
	LDR R1, [R0, #GPIO_MODER] ;Load the value from the GPIO_MODER (GPIO port mode register) into register R1. This register configures the I/O direction mode for each pin in the GPIO port.
	AND R1, R1, #(~(3<<4)) ;Perform a bitwise AND operation between the value in register R1 and the complement of the bit pattern (0x03 << 2*2). This clears the bits corresponding to the mode configuration of the GPIO pin (specifically pin 2)
	ORR R1, R1, #(1<<4) ;Perform a bitwise OR operation between the value in register R1 and the constant 0x01 shifted to the left by 2*2 bits. This sets the mode of the GPIO pin (specifically pin 2) to output mode.
	STR R1, [R0, #GPIO_MODER] ;Store the modified value back into the GPIO_MODER register to update the mode configuration for GPIO Port B pin 2.

	LDR R1, [R0, #GPIO_OTYPER] ;Load the address of GPIOB OTYPER register into a register
	BIC R1, R1, #0x4 ;Clear the 2nd bit of GPIOB OTYPER (corresponding to PB2) to configure it as push-pull
	STR R1, [R0, #GPIO_OTYPER]
	
	LDR R1, [R0, #GPIO_PUPDR] ;Load the address of GPIOB PUPDR register into a register
	BIC R1, R1, #0x30 ;Clear the 5th and 4th bits of GPIOB PUPDR (corresponding to PB2) to configure no pull-up/pull-down
	STR R1, [R0, #GPIO_PUPDR]


	LDR R0, =GPIOE_BASE ;Loads the address of the GPIOE (General-Purpose Input/Output Port E) peripheral into register R0. GPIOE_BASE is typically a symbolic constant representing the base address of the GPIOE registers.
	LDR R1, [R0, #GPIO_MODER] ;The value from the memory location specified by the sum of the value in register R0 (which holds the address of GPIOE) and the offset GPIO_MODER is loaded into register R1. GPIO_MODER is an offset representing the address of the GPIO port mode register within the GPIO peripheral, which controls the pin modes (input, output, alternate function, etc.) of the GPIO pins.
	AND R1, R1, #(~(3<<16)) ;Performs a bitwise AND operation between the value in register R1 (which holds the value read from GPIO_MODER) and the complement of the result of shifting 3 to the left by 16 bits. The 3<<16 operation is used to create a bitmask that clears the bits responsible for pin 8 mode configuration within the GPIO_MODER register, effectively masking out those bits while preserving the rest. 
	ORR R1, R1, #(1<<16) ;Bitwise OR operation is performed between the value in register R1 (after the previous AND operation) and the result of shifting 1 to the left by 16 bits. This operation sets bit 16 within the GPIO_MODER register, effectively configuring pin 8 of GPIOE as an output pin.
	STR R1, [R0, #GPIO_MODER] ;The modified value in register R1, which now reflects the updated configuration for pin 8 of GPIOE, is stored back into the memory location specified by the sum of the value in register R0 (address of GPIOE) and the offset GPIO_MODER, effectively updating the GPIO port mode register with the new value.
	
	LDR R1, [R0, #GPIO_OTYPER] ;Load the address of GPIOE OTYPER register into a register
	BIC R1, R1, #0x100 ;Clear the 9th bit of GPIOE OTYPER (corresponding to PE8) to configure it as push-pull
	STR R1, [R0, #GPIO_OTYPER]
	
	LDR R1, [R0, #GPIO_PUPDR] ;Load the address of GPIOE PUPDR register into a register
	BIC R1, R1, #0x30000 ;Clear the 17th and 16th bits of GPIOE PUPDR (corresponding to PE8) to configure no pull-up/pull-down
	STR R1, [R0, #GPIO_PUPDR]


	LDR R0, =GPIOA_BASE ;This loads the base address of the GPIOA peripheral into register R0. 
	LDR R1, [R0, #GPIO_MODER] ;Loads the value from the memory location specified by the sum of the value in register R0 (which holds the address of GPIOA) and the offset GPIO_MODER into register R1. The GPIO_MODER offset represents the address of the GPIO port mode register within the GPIO peripheral. This register controls the pin modes (input, output, alternate function, etc.) of the GPIO pins.
	AND R1, R1, #(~(3<<0)) ;Clear the bits responsible for the mode configuration of pin 0 of GPIOA within the GPIO_MODER register. Setting it to 00 which is input mode.
	AND R1, R1, #(~(3<<2)) ;Clear the mode configuration of pin 1 of GPIOA. Setting it to 00 which is input mode.
	AND R1, R1, #(~(3<<4)) ;Clears the mode configuration of pin 2 of GPIOA. Setting it to 00 which is input mode.
	AND R1, R1, #(~(3<<6)) ;Clears the mode configuration of pin 3 of GPIOA. Setting it to 00 which is input mode.
	AND R1, R1, #(~(3<<10)) ;Clears the mode configuration of pin 5 of GPIOA. Setting it to 00 which is input mode.
	STR R1, [R0, #GPIO_MODER] ;The modified value in register R1, which now reflects the updated configuration for GPIOA pins, is stored back into the memory location specified by the sum of the value in register R0 (address of GPIOA) and the offset GPIO_MODER. This updates the GPIO port mode register of GPIOA with the new value, effectively configuring multiple pins of GPIOA with the input mode.
	
	LDR R1, [R0, #GPIO_PUPDR] ; Load, in R1, the base address of GPIOA offsetted by #GPIO_PUPDR, to get to the PUPDR register of GPIOA.
	AND R1, R1, #(~(3<<2*0)) ; Reset, in R1, the 2 bits of PA 0, 1, 2, 3, and 5.
	AND R1, R1, #(~(3<<2*1))
	AND R1, R1, #(~(3<<2*2))
	AND R1, R1, #(~(3<<2*3))
	AND R1, R1, #(~(3<<2*5))
	ORR R1, R1, #(0x02<<2*0) ; Set, in R1, the 2 bits of PA 0, 1, 2, 3, 5 to 'pull-down = 10'.
	ORR R1, R1, #(0x02<<2*1)
	ORR R1, R1, #(0x02<<2*2)
	ORR R1, R1, #(0x02<<2*3)
	ORR R1, R1, #(0x02<<2*5)
	STR R1, [R0, #GPIO_PUPDR] ; Store the modified version of R1 back into GPIOA PUPDR register.
	
	B loop ;branches to the loop label
	
loop	LDR R0, =GPIOA_BASE ;Load the base of GPIOA, this is the base of the joystick
		LDR R1, [R0, #GPIO_IDR] ;Load, in R1, the Input Data Register of GPIOA, we will need this to control the joystick by giving it inputs.
	
		ANDS R1, R1, #(0x00000020) ;Bitwise AND between the value in R1 (GPIOA's IDR) and the mask 0x00000020. This isolates the bit 5 (corresponding to pressing down on the joystick) from the rest of the bits on the IDR because we can't compare the value of the enture IDR, we don't know what other bits might be doing and we don't want to concern ourselves with that.
		CMP R1, #(0x00000020) ;Compare the value of bit5-isolated IDR with 0x00000020, which is the hex value if bit 5 is activated (joystick is pressed down) - note that without the isolation the compare will never result to the expected value and the branch will never happen
		BEQ turn_off_both ;If they are equal, it branches to the turn_off_both label.
		
		LDR R0, =GPIOA_BASE ; have to repeat these two lines after every compare because we are changing the value in R1 during every ANDS isolation
		LDR R1, [R0, #GPIO_IDR]
		
		ANDS R1, R1, #(0x00000008) ;isolating bit 3 of the IDR of GPIOA
		CMP R1, #(0x00000008) ;comparing if the value of the IDR is 0x00000008 (meaning the joystick is pressed up) regardless of the value of any of the other bits besides 3
		BEQ turn_on_both ;if they are equal, branch to turn on both leds
		
		LDR R0, =GPIOA_BASE ; have to repeat these two lines after every compare because we are changing the value in R1 during every ANDS isolation
		LDR R1, [R0, #GPIO_IDR]
		
		ANDS R1, R1, #(0x00000004) ;isolating bit 2 of the IDR of GPIOA
		CMP R1, #(0x00000004) ;comparing if the value of the IDR is 0x00000004 (meaning the joystick is pressed right) regardless of the value of any of the other bits besides 2
		BEQ toggle_green ;if they are equal, branch to toggle green    
		
		LDR R0, =GPIOA_BASE ; have to repeat these two lines after every compare because we are changing the value in R1 during every ANDS isolation
		LDR R1, [R0, #GPIO_IDR]
		
		ANDS R1, R1, #(0x00000002) ;isolating bit 1 of the IDR of GPIOA
		CMP R1, #(0x00000002) ;comparing if the value of the IDR is 0x00000002 (meaning the joystick is pressed left) regardless of the value of any of the other bits besides 1
		BEQ toggle_red ;if they are equal branch to toggle red

		; Nothing happens if the middle of the joystick is pressed
		
		B loop ;If none of the specified conditions are met (i.e., no button is pressed), the code loops back to the loop label to repeat the process.
		
turn_off_both	MOV R3, #0            ; Prepare value to turn off both LEDs
				LDR R0, =GPIOB_BASE   ; Load base address of GPIOB
				STR R3, [R0, #GPIO_ODR] ; Turn off red LED
				LDR R0, =GPIOE_BASE   ; Load base address of GPIOE
				STR R3, [R0, #GPIO_ODR] ; Turn off green LED
				B loop                ; Continue looping

turn_on_both	MOV R3, #0x00000100   ; Prepare value to light up green lED
				MOV R4, #0x00000004   ; Prepare value to light up red LED
				LDR R0, =GPIOB_BASE   ; Load base address of GPIOB
				STR R4, [R0, #GPIO_ODR] ; Turn on red LED
				LDR R0, =GPIOE_BASE   ; Load base address of GPIOE
				STR R3, [R0, #GPIO_ODR] ; Turn on green LED
				B loop                ; Continue looping

toggle_green	LDR R0, =GPIOE_BASE   ; Load base address of GPIOE
				LDR R3, [R0, #GPIO_ODR] ; Load current value of GPIOE ODR
				EOR R3, R3, #(1<<8)   ; Toggle green LED (bit 8) - note that if we use ORR then it will only turn on but not toggle on and off
				STR R3, [R0, #GPIO_ODR] ; Update GPIOE ODR
				B loop                ; Continue looping

toggle_red	LDR R0, =GPIOB_BASE   ; Load base address of GPIOB
			LDR R3, [R0, #GPIO_ODR] ; Load current value of GPIOB ODR
			EOR R3, R3, #(1<<2)   ; Toggle red LED (bit 2) - note that if we use ORR then it will only turn on but not toggle on and off
			STR R3, [R0, #GPIO_ODR] ; Update GPIOB ODR
			B loop                ; Continue looping

	ENDP
	END