	INCLUDE core_cm4_constants.s     ; Include constant definitions for ARM Cortex-M4 core
	INCLUDE stm32l476xx_constants.s  ; Include constant definitions specific to STM32L476xx MCU

	AREA    main, CODE, READONLY      ; Define a code section named "main" with read-only access
	EXPORT  __main                    ; Export __main symbol to be visible to the linker
	ENTRY                             ; Mark the beginning of the program

__main  PROC   ; Start of the main procedure
    ; Enable the clock to GPIO Port B
    LDR r0, =RCC_BASE             		; Load the address of RCC (Reset and Clock Control) peripheral base
    LDR r1, [r0, #RCC_AHB2ENR]    		; Load the value of r0 plus the constant offset RCC_AHB2ENR register into r1
    ORR r1, r1, #RCC_AHB2ENR_GPIOBEN 	; Set GPIOBEN bit in RCC_AHB2ENR register
    STR r1, [r0, #RCC_AHB2ENR]    		; Store the modified value back to RCC_AHB2ENR register

    ; Configure GPIO Port B Pin 2 as output
    LDR r0, =GPIOB_BASE           ; Load the address of GPIOB peripheral base
    LDR r1, [r0, #GPIO_MODER]     ; Load the value of GPIOB_MODER register into r1
    EOR r1, r1, #(3<<4)    		  ; Clear bits 5:4 (Pin 2 mode bits)
    ORR r1, r1, #(1<<4)           ; Set bit 4 to configure Pin 2 as output
    STR r1, [r0, #GPIO_MODER]     ; Store the modified value back to GPIOB_MODER register

    ; Set Pin 2 output high
    LDR r1, [r0, #GPIO_ODR]       ; Load the value of GPIOB_ODR register into r1
    ORR r1, r1, #(1<<2)           ; Set bit 2 to set Pin 2 output high
    STR r1, [r0, #GPIO_ODR]       ; Store the modified value back to GPIOB_ODR register

stop    B   stop                  ; Branch to stop (infinite loop)
	ENDP                          ; End of main procedure

	ALIGN                              ; Align subsequent code/data on a word boundary

	AREA    myData, DATA, READWRITE    ; Define a data section named "myData" with read-write access
	ALIGN                              ; Align subsequent data on a word boundary
array   DCD   1, 2, 3, 4               ; Define an array named "array" with initial values 1, 2, 3, 4
	END                                ; End of the program