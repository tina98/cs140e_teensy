/*
 *  blinky.c for the Teensy 3.1 board (K20 MCU, 16 MHz crystal)
 *
 *  This code will blink the Teensy's LED.  Each "blink" is
 *  really a set of eight pulses.  These pulses give the actual
 *  system clock in Mhz, starting with the MSB.  A pulse is
 *  narrow for a 0-bit and wide for a 1-bit.
 *
 *  For a system clock of 72 MHz, blinks will read 0x48.
 *  For a system clock of 48 MHz, blinks will read 0x30.
 */

#include  "common.h"
#include "cs140e-teensy-gpio.h"
#include "cs140e-teensy-uart.h"

#include <stdio.h>
#include<stdlib.h>
#include<string.h>

#define  LED_ON		GPIOC_PSOR=(1<<5)
#define  LED_OFF	GPIOC_PCOR=(1<<5)

volatile unsigned int interrupt_inc = 0;


void PORTD1Handler() {
	interrupt_inc++;
}
void PORTD_IRQHandler() {
	if( PORTD_ISFR & (1))
		PORTD1Handler();
}

void delay_us(unsigned us) {
    // unsigned rb = timer_get_time();
    uint32_t rb = (uint32_t)mcg_clk_hz;
	rb = rb / 1000000;
    // unsigned rb = mcg_clk_hz;
    while (1) {
        // unsigned ra = timer_get_time();
        uint32_t ra = (uint32_t)mcg_clk_hz;
		ra =ra / 1000000;
        // volatile unsigned ra = mcg_clk_hz;
        if ((ra - rb) >= us) {
            return;
        }
    }
}

void soft_uart_transmit(unsigned char txpin, unsigned us_per_bit, const char* str, unsigned string_size) {
	unsigned i=0, bit = 0;
	for (i = 0; i<string_size; i++){
		char c = str[i];

		gpio_write(txpin, 0);
		delay_us(us_per_bit);

		for (bit =0; bit<sizeof(char) * 8; bit++){
			gpio_write(txpin, (unsigned char) ((c>>bit)&1));
			delay_us(us_per_bit);
		}

		gpio_write(txpin, 1);
		delay_us(us_per_bit);
	}
}
void timer_init() {
	RTC_SR = 0b10000;
	// if (!(RTC_CR & RTC_CR_OSCE)) {
	// 	RTC_SR = 0;
	// 	RTC_CR =  RTC_CR_OSCE;
	// }

	// if (RTC_SR) {
	// 	// enable the RTC
	// 	RTC_SR = 0;
	// 	RTC_TPR = 0;

	// 	// RTC_TSR = TIME_T;
	// 	// RTC_SR = RTC_SR_TCE;
	// }
}


// Pin 13 = LED (C5)
// Pin 14 = sensor (D1)
// Pin 0 = receive (B16)
// Pin 1 = transmit (B17)
// int led = ;
int main(void)
{
	gpio_init();
	uart_init(115200);

	volatile uint32_t			n;

	gpio_set_output(12);
	gpio_set_output(13);
	gpio_set_output(14);



	gpio_write(13,1);


	volatile int i = 0;
	while(1) {
		if (i%2) {
			uart_println("hello world.\n");
		} else {
			uart_println("hello world..\n");
		}
		i = (i+1)%1000;
		gpio_write(14, 1);
		for (n=0; n<1000000; n++)  ; // stupid loop
		gpio_write(14, 0);
		for (n=0; n<1000000; n++)  ;

	}


	return  0;						// should never get here!
}
