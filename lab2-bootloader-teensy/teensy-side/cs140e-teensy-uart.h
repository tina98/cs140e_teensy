#ifndef __UART_H__
#define __UART_H__

void uart_init();

void uart_send_byte(unsigned data);
unsigned int uart_get_byte();
unsigned int uart_try_get_byte(int try);
void uart_send_int(unsigned data);
unsigned int uart_get_int();

void uart_println(char* data);
void delay();

void error_light();
void error_light_off();

void led();
void led_off();

void BRANCHTO( unsigned addr);
void dummy ( unsigned int );
#endif