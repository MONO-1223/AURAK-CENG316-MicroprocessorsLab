#include "stm32l476xx.h"
#include "lcd.h"

void System_Clock_Init(void);
void keypad_pin_init(void); // initialize GPIO pins for keypad
void delay_ms(unsigned int t); // rudimentary delay millisecond function
void clear_row_output(void); // clear PE 10, 11, 12, 13 output bits
void set_row_output(void); // set PE 10, 11, 12, 13 output bits
void drive_row_low(char row); // set all row bits except and clear specified row
signed char read_column_input(char col);
signed char map_key(signed char i); // map the row and column index to corresponding character
signed char get_key(void); // scan in key press
char debounce(char col); // debounce key press

int main(void) {
char str[50];
char *ptr;
signed char key, mapped_key, previous_key;
unsigned char str_len;
unsigned char delete_pressed;
ptr = str;
str_len = 0;
mapped_key = '0';
previous_key = '0';
delete_pressed = 0;
System_Clock_Init();
LCD_Initialization();
LCD_Clear();
keypad_pin_init();
while(1) {
while ((key = get_key()) == -1); // wait until there is a key pressed
mapped_key = map_key(key); // map the key to actual character
switch (mapped_key) {
case '*':
// '*' character deletes last character
*--ptr = 0;
str_len--;
break;
case 'P':
// '#' character displays previous key pressed
*ptr++ = previous_key;
str_len++;
break;
default:
// add key to the string
*ptr++ = mapped_key;
str_len++;
previous_key = mapped_key;
}
if (str_len <= 6) {
LCD_DisplayString((uint8_t *)str);
} else {
// move the displayed string to see newest characters
LCD_DisplayString((uint8_t *)str+str_len-6);
}
while (get_key() != -1) {
// clear the LCD if '*' is pressed for a while
if (mapped_key == '*') {
delete_pressed++;
}
if (delete_pressed > 100) {
LCD_Clear();
while (str - ptr) {
*--ptr =0;
}
str_len = 0;
delete_pressed = 0;
}
}
}
}

// scan in key press
signed char get_key(void) {
int i, j; // these are loop counters
char pressed = 0; // a flag to indicate whether a key is pressed
clear_row_output(); // first clear all output bits
for (i = 0; i < 4; i++) { // scanning the columns
if (read_column_input(i) == 0) {
if (debounce(i)) { // debounce if key is pressed
pressed = 1;
break;
}
}
}

if (pressed == 1) {
// If a key is pressed, it enters another loop over the rows.
// It sets each row low one at a time using drive_row_low(j).
// It checks if the key is still pressed by reading the column input again.
// If the key is still pressed, it calculates and returns the index of the pressed key based on both row and column.
for (j = 0; j < 4; j++) {
drive_row_low(j);
delay_ms(1); // bits need some time to change
if (read_column_input(i) == 0) {
clear_row_output(); // clear output rows when done
return 4*j + i;
}
}
}
clear_row_output();
return -1; // return -1 if no key press detected
}

// debounce key press
char debounce(char col) {
char k;
char counter = 0; // number of times key is pressed
char debounce_count = 5; // threshold for good key pres
// check key state ten times over ten milliseconds
for (k = 0; k < 10; k++) {
if (read_column_input(col) == 1 && counter > 0) {
counter--;
}
if (read_column_input(col) == 0) {
counter++;
}
delay_ms(1);
}
if (counter >= debounce_count) {
return 1;
} else {
return 0;
}
}

// map the row and column index to corresponding character
signed char map_key(signed char i) {
char keys[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', 'P', 'D'};
if (i > 15 || i < 0) {
return -1;
} 
else {
return keys[i];
}
}

signed char read_column_input(char col) {
switch (col) {
case 0: return GPIOA->IDR & 1<<1;
case 1: return GPIOA->IDR & 1<<2;
case 2: return GPIOA->IDR & 1<<3;
case 3: return GPIOA->IDR & 1<<5;
default:
return -1;
}
}

// set all row bits except and clear specified row
// the rows are read indirectly through the drive_row_low function, which sets one row low at a time and then checks the columns for a pressed key.
void drive_row_low(char row) {
set_row_output();
switch (row) {
case 0: GPIOE->ODR &= ~0x0400; break;
case 1: GPIOE->ODR &= ~0x0800; break;
case 2: GPIOE->ODR &= ~0x1000; break;
case 3: GPIOE->ODR &= ~0x2000; break;
}
}

// clear PE 10, 11, 12, 13 output bits
void clear_row_output(void) {
GPIOE->ODR &= ~0x3C00;
}

// set PE 10, 11, 12, 13 output bits
void set_row_output(void) {
GPIOE->ODR |= 0x3C00;
}

// rudimentary delay millisecond function
void delay_ms(unsigned int t) {
unsigned int i, j;
for (i = 0; i < t; i++) {
for (j = 0; j < 2000; j++);
}
}

// initialize GPIO pins for keypad
void keypad_pin_init(void) {
// R1->PE10 R2->PE11 R3->PE12 R4->PE13
// C1->PA1 C2->PA2 C3->PA3 C4->PA5
// enable GPIO Port A and E clock
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
// Set Port E pin 10, 11, 12, 13 as OUTPUT
GPIOE->MODER &= ~0x0FF00000; // these values are calculated in the prelab section of the report
GPIOE->MODER |= 0x05500000;
// No pull up no pull down
GPIOE->PUPDR &= ~(3U<<20 | 3U<<22 | 3U<<24 | 3U<<26);
// Open drain to prevent short circuit when multiple keys pressed
GPIOE->OTYPER |= 1U<<10 | 1U<<11 | 1U<<12 | 1U<<13;
// Set PORT A pin 1, 2, 3, 5 as INPUT
GPIOA->MODER &= ~0x00000CFC;
GPIOA->MODER |= 0x00000000;
// No pull up no pull down
GPIOA->PUPDR &= ~(3U<<2 | 3U<<4 | 3U<<6 | 3U<<10);
}

void System_Clock_Init(void){
// Enable High Speed Internal Clock (HSI = 16 MHz)
RCC->CR |= ((uint32_t)RCC_CR_HSION);
// wait until HSI is ready
while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );
// Select HSI as system clock source
RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI; //01: HSI16 oscillator used as system clock
// Wait till HSI is used as system clock source
while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );
}