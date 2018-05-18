/*
 * Lab 1 Design 2.c
 *
 * Created: 5/11/2018 1:59:43 PM
 * Author : Group 27
 */ 

#include "uart.h"
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TIMER_SPEED	(F_CPU/8)
/*
	PIN Change ISR for PC0.
	Measures the frequency and duty cycle of PWM waveforms and outputs the values
	over UART Baud 115200.	
*/
ISR(PCINT2_vect)
{
	static uint16_t t1;
	static uint16_t t2;
	static uint16_t delta_t;
	static uint16_t t_high;
	static uint16_t t_duty;
	static uint16_t duty;
	static uint8_t falling = 0;
	static uint8_t first_rise = 1;
	static uint16_t freq;
		
	if(PINC & 0x01) {
		if(first_rise) {
		/* This handles the first rise of a PWM */
			t1 = ((uint16_t)TCNT1L) | ((uint16_t)TCNT1H<<8);
			first_rise = 0;
			falling = 1;
		}
		else if(!falling) {
		/* this handles the second rise of a period of a PWM */
			t2 = ((uint16_t)TCNT1L) | ((uint16_t)TCNT1H<<8);
			delta_t = t2 - t1;
			t_duty = t_high - t1;
			duty = (uint16_t)(((float)t_duty/delta_t)*100.);
			freq = (uint16_t)(TIMER_SPEED/delta_t);
			printf("\nt1 = %u\nt2 = %u\ndelta_t = %u\nt_high = %u\nt_duty = %u\nduty = %u\nfreq = %u\n",t1,t2,delta_t,t_high,t_duty,duty,freq);
			first_rise = 1;
			_delay_ms(2500);
		}
		else {
		/*	This handles if an interrupt is missed
			This will reset the pulse we are measuring */
			first_rise = 1;
			falling = 0;
			
		}
	}
	else if(falling){
	/* This handles the falling edge of the PWM period */
		t_high = ((uint16_t)TCNT1L) | ((uint16_t)TCNT1H<<8);
		falling = 0;
	}
}

int main(void)
{
	init_uart();
/*	Set DDR */
	DDRB |= (1<<PB3) | (1<<PB4);	
	DDRC &= (0<<PC0);
/*	Configure Fast PWM Mode	*/
	TCCR0A |= (1<<WGM00) | (1<<WGM01) | (1<<COM0B1) | (1<<COM0A1);
	TCCR0B |= (1<<CS01);
/*	Set one wave to have 25% duty cycle and one to have 75% duty cycle */
	OCR0A = 64;
	OCR0B = 191;
/*	Enable TIMER1 with prescale factor 8 */
	TCCR1B = (1<<CS11);
/*	Enable PCINT on PC0 */
	PCICR |= (1<<PCIE2);			
	PCMSK2 |= (1<<PCINT16);
	sei(); 
	
	_delay_ms(1000);
	
	while(1)
	{
	}
	 

}


