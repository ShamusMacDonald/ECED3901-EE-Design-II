
#include "uart.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#define BAUDRATE	(uint16_t)((F_CPU/16UL/BAUD)-1)	// Baud eqn from datasheet

// Allows std streams for I/O
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar,NULL,_FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL,uart_getchar,_FDEV_SETUP_READ);

int uart_putchar(char c, FILE *stream)
{
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = c;
	return 0;
}

int uart_getchar(FILE *stream)
{
	loop_until_bit_is_set(UCSR0A,RXC0);
	return UDR0;
}

void init_uart()
{	
	UBRR0H = (unsigned char)(BAUDRATE>>8);	// Set baud
	UBRR0L = (unsigned char)(BAUDRATE);
	
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);	// 8 bit data frame
			
	stdout = &mystdout;
	stdin = &mystdin;
	
	printf("Atmega644p UART Connection Established...\n\n");
	_delay_ms(1000);
}