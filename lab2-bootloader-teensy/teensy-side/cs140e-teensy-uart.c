// code from https://github.com/Suyash458/Teensy_C_Samples/blob/master/examples/uart/uart.c
#include <stddef.h>
#include <stdint.h>
#include  "K20P64.h"
#include "cs140e-teensy-uart.h"



void PUT8 ( unsigned int, unsigned int );
void PUT16 ( unsigned int, unsigned int );
void PUT32 ( unsigned int, unsigned int );
unsigned int GET8 ( unsigned int );
unsigned int GET16 ( unsigned int );
unsigned int GET32 ( unsigned int );

void ASMDELAY ( unsigned int );

// If there's something to send (masks), send the byte
void uart_send_byte(unsigned data) {
    while(!(GET8(UART0_S1)&UART_TDRE_MASK)|| !(GET8(UART0_S1)&UART_TC_MASK)) {}
    PUT8(UART0_D, data);
}

// If there's something to receive, read the byte
unsigned int uart_get_byte() {
	PUT8(UART0_S1, SET(3));
	while(!(GET8(UART0_S1) & UART_RDRF_MASK))
		;

	return GET8(UART0_D);

}

// try to get the byte or timeout
int ledd=0;
unsigned int uart_try_get_byte(int try) {
    PUT8(UART0_S1, SET(3));
    ledd = ledd%2;
    int test = 0;
    if (ledd%2)
        error_light();
    else
        error_light_off();
    while(!(GET8(UART0_S1) & UART_RDRF_MASK)) {
        if (test>=try) {
            ledd++;
            return 0;
        }
        test++;
    }


    return GET8(UART0_D);

}

// simple debugging LED lights turning on/off
void error_light() {
    PUT32(GPIOC_PSOR, 1<<5);
}

void error_light_off() {
    PUT32(GPIOC_PCOR, 1<<5);
}

void led() {
    PUT32(GPIOC_PSOR, 1<<7);
}

void led_off() {
    PUT32(GPIOC_PCOR, 1<<7);
}

// Send/get int (4 bytes)
void uart_send_int(unsigned data) {
    uart_send_byte((data >> 0)  & 0xff);
    uart_send_byte((data >> 8)  & 0xff);
    uart_send_byte((data >> 16) & 0xff);
    uart_send_byte((data >> 24) & 0xff);
        return;
}

unsigned int uart_get_int() {
	unsigned u = uart_get_byte();
        u |= uart_get_byte() << 8;
        u |= uart_get_byte() << 16;
        u |= uart_get_byte() << 24;
	return u;
}

// Send an entire string
void uart_println(char* data) {
	char* c = data;
	while (*c) {
		uart_send_byte(*c++);
		delay();
	}
	uart_send_byte('\n');
	delay();
}

// Delay by checking the timer current value
void delay ( void )
{
    unsigned int ra;

    for(ra=0;ra<1;ra++)
    {
        while(1) if(GET32(STK_CVR)&0x800000) break;
        while(1) if((GET32(STK_CVR)&0x800000)==0) break;
    }
}

// Initalize all the GPIO/timer/UART registers
void uart_init()
{

    //Turn off watchdog and allow update
    PUT16(WDOG_UNLOCK,0xC520);
    PUT16(WDOG_UNLOCK,0xD928);
    PUT16(WDOG_STCTRLH,0x0010);

    //Enable GPIOC
    PUT32(SIM_SCGC5,GET32(SIM_SCGC5)|(1<<11));

    //configure Port C5
    PUT32(PORTC_PCR5,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<5));

    //configure Port C5
    PUT32(PORTC_PCR7,(1<<8));
    PUT32(GPIOC_PDDR,GET32(GPIOC_PDDR)|(1<<7));

    //switch to 96Mhz
    //start in FEI mode FLL Engaged Internal
    PUT8(OSC0_CR,0xA);
    PUT8(MCG_C2,(2<<4)|(1<<2));
    PUT8(MCG_C1,(2<<6)|(4<<3));
    while ((GET8(MCG_S) & (1<<1)) == 0) continue;
    while ((GET8(MCG_S) & (1<<4)) != 0) continue;
    while (((GET8(MCG_S)>>2)&3) != 2) continue ;
    //FBE mode FLL Bypassed External
    PUT8(MCG_C5,(3<<0));
    PUT8(MCG_C6,(1<<6)|(0<<0));
    while (!(GET8(MCG_S) & (1<<5))) continue;
    while (!(GET8(MCG_S) & (1<<6))) continue;
    // PBE mode PLL Bypassed External
    PUT32(SIM_CLKDIV1,(0<<28)|(1<<24)|(3<<16));
    PUT8(MCG_C1,(0<<6)|(4<<3));
    while (((GET8(MCG_S)>>2)&3)!=3) continue;
    // PEE mode PLL Engaged External
    PUT32(SIM_CLKDIV2,(1<<1));
    PUT32(SIM_SOPT2,(1<<18)|(1<<16)|(1<<12)|(6<<5));

    // set timer divider
    PUT32(SIM_SCGC4,GET32(SIM_SCGC4)|(1<<10));
    PUT32(SIM_SCGC5,GET32(SIM_SCGC5)|(1<<10));
    PUT32(PORTB_PCR17,(3<<8)|(1<<6));
    PUT32(PORTB_PCR16,(3<<8)|(1<<6));
    PUT8(UART0_BDH,(1667>>13)&0x1F);
    PUT8(UART0_BDL,(1667>> 5)&0xFF);
    PUT8(UART0_C4, (1667>> 0)&0x1F);

    // Set transmitter
    PUT8(UART0_C2,0xC);

    // set timer off, set reload value to max, then enable timer
    PUT32(STK_CSR,0x00000004);
    PUT32(STK_RVR,0xFFFFFFFF);
    PUT32(STK_CSR,0x00000005);


}