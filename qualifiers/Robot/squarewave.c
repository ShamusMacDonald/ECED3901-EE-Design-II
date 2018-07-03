/*
 * squarewave.c
 *
 * Created: 7/2/2018 1:46:45 PM
 *  Author: Shamus
 */ 

#include "squarewave.h"

#include <stdio.h>

void squareWaveGen(char timer, char timerPort, float dutyCycle)
{	
	float dutyFraction;
	
	switch(timer) 
	{ // this is a case switch for which timer is being used to generate the square wave
		case TIMER0 :
			dutyFraction = dutyCycle/100 * 255; // hash define this 255
			TCCR0A |= (1<<WGM00) | (1<<WGM01); // sets fast PWM mode
			TCCR0B |= (1<<CS01) ; // sets a pre-scale of 8
			if(timerPort == 0) {
				OCR0A = (uint16_t)dutyFraction; // sets appropriate duty cycle
				TCCR0A |= (1<<COM0A1); // sets port A
				DDRB |= (1<<PB3);  // setting pins to output
			} 
			else if (timerPort == 1) {
				TCCR0A |= (1<<COM0B1); // sets port B
				OCR0B = (uint16_t)dutyFraction; // sets appropriate duty cycle
				DDRB |= (1<<PB4);  // setting pins to output
			} else {
				OCR0A = (uint16_t)dutyFraction; // sets appropriate duty cycle
				OCR0B = (uint16_t)dutyFraction; // sets appropriate duty cycle
				DDRB |= (1<<PB3) | (1<<PB4);  // setting pins to output
				TCCR0A |= (1<<COM0A1) | (1<<COM0B1); // sets both ports
			}
			break;
		case TIMER1 :
			dutyFraction = dutyCycle/100. * 65536; // hash define this 65000
			ICR1L = 0xFF;
			ICR1H = 0xFF;
			TCCR1A |= (0<<WGM10) | (1<<WGM11); // sets fast PWM mode
			TCCR1B |= (1<<CS11) | (1<<WGM12) | (1<<WGM13) ; // sets a pre-scale of 8
			if(timerPort == 0){
				//OCR1AL = (uint16_t)dutyFraction; // sets appropriate duty cycle
				//OCR1AH = (uint16_t)dutyFraction>>8;
				OCR1AL = 0x00;
				OCR1AH = 0x40;
				TCCR1A |= (1<<COM1A1); // sets port A
				DDRD |= (1<<PD5); // setting pins to output
			}
			else if(timerPort == 1){
				OCR1BL = (uint16_t)dutyFraction; // sets appropriate duty cycle
				OCR1BH = (uint16_t)dutyFraction>>8;
				TCCR1A |= (1<<COM1B1);// sets port B
				DDRD |= (1<<PD4); // setting pins to output
			}
			else{
				OCR0A = (uint16_t)dutyFraction; // sets appropriate duty cycle
				OCR0B = (uint16_t)dutyFraction; // sets appropriate duty cycle
				TCCR1A |= (1<<COM1A1) | (1<<COM1B1); // sets both ports
				DDRD |= (1<<PD5) | (1<<PD4); // setting pins to output
			}
			break;
		case TIMER2 :
			dutyFraction = dutyCycle/100 * 255;	
			TCCR2A = (1<<WGM20) | (1<<WGM21);		// sets fast PWM mode
			TCCR2B = (1<<CS21) ;					// sets a pre-scale of 8
			if(timerPort == 0) {
				OCR2A = (uint16_t)dutyFraction;
				TCCR2A |= (1<<COM2A1); // sets port A
				DDRD |= (1<<PD7); // setting pins to output
			}
			else if(timerPort == 1) {
				TCCR2A |= (1<<COM2B1);				// sets port B
				OCR2B = (uint16_t)dutyFraction;
				DDRD |= (1<<PD6);					// setting pins to output
			}
			else{
				OCR2A = (uint16_t)dutyFraction;
				OCR2B = (uint16_t)dutyFraction;
				TCCR2A |= (1<<COM2A1) | (1<<COM2B1);	// sets both ports
				DDRD |= (1<<PD7) | (1<<PD6);			// setting pins to output
			}
			break;
		default: // let them know they screwed up
			break;
	}
}