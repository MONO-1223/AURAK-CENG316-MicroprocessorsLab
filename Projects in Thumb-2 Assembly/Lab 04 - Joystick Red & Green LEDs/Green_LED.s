	INCLUDE core_cm4_constants.s  
	INCLUDE stm32l476xx_constants.s  

	AREA    main, CODE, READONLY      
	EXPORT  __main                   
	ENTRY                             

__main  PROC 
    LDR r0, =RCC_BASE             		
    LDR r1, [r0, #RCC_AHB2ENR]    		
    ORR r1, r1, #RCC_AHB2ENR_GPIOEEN  ; Change
    STR r1, [r0, #RCC_AHB2ENR]    		

    LDR r0, =GPIOE_BASE  ; Change       
    LDR r1, [r0, #GPIO_MODER]     
    EOR r1, r1, #(3<<16)  ; Change   		 
    ORR r1, r1, #(1<<16)  ; Change        
    STR r1, [r0, #GPIO_MODER]    

    LDR r1, [r0, #GPIO_ODR]     
    ORR r1, r1, #(1<<8)  ; Change       
    STR r1, [r0, #GPIO_ODR]   

stop    B   stop                 
	ENDP                         

	ALIGN                             

	AREA    myData, DATA, READWRITE   
	ALIGN                              
array   DCD   1, 2, 3, 4               
	END                                