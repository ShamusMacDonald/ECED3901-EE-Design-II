/*
 * Lab 1 Design 2.c
 *
 * Created: 5/11/2018 1:59:43 PM
 * Author : Group 27
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

ISR(PCINT2_vect)
{
	if(PINC & 0x01)
	{
		//Configure Fast PWM Mode
		TCCR0A |= (1<<WGM00) | (1<<WGM01) | (1<<COM0B1) | (1<<COM0A1) ;
		TCCR0B |= (1<<CS01) ;
		//Set one wave to have 25% duty cycle and one to have 75% duty cycle
		OCR0A = 64;
		OCR0B = 192;
	}
	else {
		TCCR0A &= (0<<WGM00) | (0<<WGM01) | (0<<COM0B1) | (0<<COM0A1) ;
		TCCR0B &= (0<<CS01) ;
	}
}

int main(void)
{
	init_uart();
	//Set Data Direction Bits for Output
	DDRB |= (1<<PB3) | (1<<PB4) ;
	DDRC &= (0<<PC0);

	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT16);
	sei(); // enables global interrupts
	
	while(1)
	{
	}
	 

}

