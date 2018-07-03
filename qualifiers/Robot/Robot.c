/*
 * DesignLab3.c
 *
 * Created: 6/5/2018 5:13:46 PM
 * Author : Shamus
 */ 

#define F_CPU	14745600UL
//#define DEBUG_CONTROL	0


#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "squarewave.h"
#include "metaldetector.h"

#define FORWARDS	1
#define BACKWARDS	0

#define RIGHT		0
#define LEFT		1
#define RIGHT_LEFT	2

#define FORSPEED	75.
#define BACKSPEED	25.
#define BREAK		50.

#define METAL_DUTY		50.
#define METAL_OUTPUT	1		// PD6 , OC2B
#define UNTIL_METAL		-1
#define UNTIL_NO_METAL	-2

#define DT_MS	250
#define DT		0.250

#define KP		4
#define KI		8
#define KD		0

volatile int right = 0;
volatile int left = 0;
volatile int right_total = 0;
volatile int left_total = 0;
static float no_metal;
static float metal_level;

//pin change interrupt, (LEFT WHEEL PB0)
ISR(PCINT1_vect) //this corresponds to PORTB Pin 0
{
	left++;
	left_total++;
}

//pin change interrupt, (RIGHT WHEEL PC0)
ISR(PCINT2_vect) //this corresponds to PORTC Pin 0
{
	right++;
	right_total++;
}

void PIDcontrol(uint16_t direction)
{
	int error;
	static int prev_err;
	static float integral;
	float derivative;
	static int total_err;
		
	left = 0;
	right = 0;
	_delay_ms(DT_MS);
	
	error		=	left - right; 						// +ve error, speed right up, -ve error slow right down
	integral	+=	error * DT;
	derivative	=	(error - prev_err)/DT;
	
	total_err += error;
//	OCR0A += KP * total_err;
	if(direction == FORWARDS) {	
		OCR0B += KP * error + KI * integral + KD * derivative;
	}
	else if(direction == BACKWARDS) {
		OCR0B -= KP * error + KI * integral + KD * derivative;
	} // else do nothing invalid parameter
	prev_err = error;
	
	#ifdef DEBUG_CONTROL
	printf("error\ttot_err\tprop\tinteg\tleft_t\tright_t\n");
	printf("%d\t%d\t%d\t%7.3f\t%d\t%d\n",error,total_err,KP*error,KI*integral,left,right); //reset Left and Right wheel ticks
	#endif
}

void drive(int dist, uint16_t direction)
{	
	left_total = 0;
	right_total = 0;
//	wait until the required distance has been traveled
	PORTC &= ~( (1 << PC4) | (1 << PC5) );
	if(direction == FORWARDS) {
		squareWaveGen(TIMER0,2,FORSPEED);
	}
	else if(direction == BACKWARDS) { //Backwards
		squareWaveGen(TIMER0,2,BACKSPEED);
	}
	
	if(dist == UNTIL_METAL) {
	//	Drive until we detect metal
		while(read_adc(PA0) > metal_level) {
			PIDcontrol(direction);
		}
	}
	else if(dist == UNTIL_NO_METAL) {
		while(read_adc(PA0) < metal_level) {
			PIDcontrol(direction);
		}
	}
	else {
		while(left_total <= dist) {
			PIDcontrol(direction);
		}
	}
//	Break
	PORTC |= (1<<PC4) | (1<<PC5);
	squareWaveGen(TIMER0,2,BREAK);
}

void turn(uint16_t degrees, uint16_t direction)
{
	left_total = 0;
	right_total = 0;
	PORTC &= ~( (1 << PC4) | (1 << PC5) );
	while(left_total <= degrees)
	{
		switch(direction)
		{
			case LEFT:
				squareWaveGen(TIMER0,LEFT,FORSPEED);
				squareWaveGen(TIMER0,RIGHT,BACKSPEED);
				break;
			case RIGHT:
				squareWaveGen(TIMER0,RIGHT,FORSPEED);
				squareWaveGen(TIMER0,LEFT,BACKSPEED);
				break;
		}	
	}
	PORTC |= (1<<PC4) | (1<<PC5);
	squareWaveGen(TIMER0,2,BREAK);
}

int main(void)
{	
	DDRB &= (0<<PB0);				// PB0 is left sensor input
	DDRC &= (0<<PC0);				// PC0 is right sensor input
	DDRC |= (1<<PC4) | (1<<PC5);
	
	PORTC |= (1<<PC4) | (1<<PC5);
	squareWaveGen(TIMER0,2,BREAK);	// Start motors off
	
//	this is setting up PB0 and PC0 as the input interrupts for the sensors
	PCICR	=	(1<<PCIE1) | (1<<PCIE2);
	PCMSK1	=	(1<<PCINT8); 				//this corresponds to PORTB pin 0
	PCMSK2	=	(1<<PCINT16);				//this correspondsto PORTC pin 0
	sei();
	init_uart();
	init_adc();
	
	squareWaveGen(TIMER2,METAL_OUTPUT,METAL_DUTY);
	TCCR2B = (1<<CS20);	// 57.6 kHz

	_delay_ms(3000);
	
	no_metal = read_adc(PA0);
	metal_level = 3 * no_metal / 4;
//	printf_P(PSTR("%7.3f\t%7.3f\n"),no_metal,metal_level);
	
//	drive(32,BACKWARDS);
//	_delay_ms(100);

	#ifndef DEBUG_ADC
	drive(UNTIL_METAL,FORWARDS);
	_delay_ms(1000);
	turn(6,RIGHT);
	_delay_ms(1000);
	drive(UNTIL_NO_METAL,FORWARDS);
	_delay_ms(1000);
	turn(6,RIGHT);
	_delay_ms(1000);
	drive(32,FORWARDS);
	#endif
	
	while(1) {
		#ifdef DEBUG_ADC
		read_adc(PA0);
		#endif
		_delay_ms(1000);
	}
}