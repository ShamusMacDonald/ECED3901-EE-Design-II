/*
 * DesignLab3.c
 *
 * Created: 6/5/2018 5:13:46 PM
 * Author : Shamus
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

#include "uart.h"

int right=0;		//count right wheel ticks
int left=0;			//count left wheel ticks
int Left=0;			//count left wheel ticks 2
int Right=0;		//count right wheel ticks 2
float dist=0;		//initial distance traveled
int del=200;		//delay
float v_left=0;		//initial velocity of left wheel
float v_right=0;	//initial velocity of right wheel
float angle=0;		//initial bearing
float x=0;			//initial position
float y=0;			//initial position
float length=0.14;	//the distance between wheels in meters
int sample=30;		//sample time in tens of ms
float error=0;		//velocity error
int error2=0;
volatile int count;
volatile float range;
float dist_fwd = 0;

void Delay(unsigned int Delay)
{	//this function delays for 10ms inherently Delay*10ms in total
	int i;
	for(i=0;i<Delay;i++)
	{
		TCCR1A=0;TCCR1B=(1<<CS12)|(1<<CS10);
		OCR1A=144;
		TCNT1=0;
		TIFR1=(1<<OCF1A);
		while(!(TIFR1&(1<<OCF1A)));
	}
	TCCR1B=0;
}

//pin change interrupt, (LEFT WHEEL PB0)
ISR(PCINT1_vect) //this corresponds to PORTB Pin 0
{
	left=left+1;
	Left=Left+1;
}

//pin change interrupt, (RIGHT WHEEN PC0)
ISR(PCINT2_vect) //this corresponds to PORTC Pin 0
{
	right=right+1;
	Right=Right+1;
}

void Control(void)
{
	Delay(sample);//delay to accumulate data
	
	dist=0.5*((left*0.012763)+(right*0.012763)); 		//avg distance
	v_left=(Left*0.012763)/(sample*0.01); 		 		//left wheel velocity
	v_right=(Right*0.012763)/(sample*0.01); 	 		//right wheel velocity
	angle=angle+(v_right-v_left)*(sample*0.01)/length; 	//bearing
	x=x+0.5*(v_left+v_right)*cos(angle)*(sample*0.01); 	//relative x position
	y = y + 0.5*(v_left+v_right)*sin(angle)*(sample*0.01); 	//relative y position
	error = v_left - v_right; 								//+ve error, speed right up, -ve error slow right down
	error2 = left-right;
	
	if(error>0)
	{	//this is speed up
		OCR2A++;	//	PD7
		//OCR2B--;	//	PD6
	}
	else if(error<0)
	{	//this is slow down
		OCR2A--;	//	PD7
		//OCR2B++;	//	PD6
	}
	else if(error==0)
	{
		//OCR2A=OCR2A;
		//OCR2B=OCR2B;
	}
	printf("ErrorTick\tError\tLeft_Motor\tRight_Motor\tLeft_Speed\tRight_Speed\tBearing\n");
	printf_P(PSTR("%7.3f\t\t%d\t\t%d\t%d\t\t%7.3f\t\t%7.3f\t\t%7.3f\n"),error,error2,OCR0A,OCR2A,v_left,v_right,angle*57.3); //reset Left and Right wheel ticks
	Left=0;
	Right=0;
}

void forward(float dist_f)
{	//	Left Motor IN1=PD7 IN2=PD6, using TIMER0
	//	Right Motor IN1=PB4 IN2=PB3, using TIMER2, OCR varies for this motor
	TCCR0A = 0;
	TCCR0A = (1<<COM0A1)|(1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B = (1<<CS01);
	TCNT0=0;
	OCR0A=64; 	//PB3output
	OCR0B=64; 	//PB4 output
	// 	OCRxA at 64 equals a 7.2kHz square wave with a 25.4% duty cycle
	TCCR2A=(1<<COM2A1)|(1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
	TCCR2B=(1<<CS21);
	TCNT2=0;
	OCR2A=80; //PD7 output
	OCR2B=80; //PD6 output
	//waits until the required distance has been traveled
	while(dist <= dist_f) {
		Control();
	}
	TCCR0B=0x00; //turn timers 0 and 2 off
	TCCR2B=0x00;
	TCCR0A=0x00;
	TCCR2A=0x00;
	PORTD=0b10000000; //brake
	PORTB=0b00010000; //brake
	//	reset all the variables
	left=0;
	right=0;
	dist=0;
	angle=0;
	x=0;
	y=0;
	v_left=0;
	v_right=0;
}

int main(void)
{
	DDRB=0x18;  //PB0 is left sensor input, PB3 and PB4 are PWM out for left motor
	DDRC=0x00;  //PC0 is the right sensor input
	DDRD=0xF0; 	//PD6 and PD7 are outputs for the right motor
	PORTB=0x18;
	PORTD=0xC0;
	count=0;	
//	this is setting up PB0 and PC0 as the input interrupts for the sensors
	PCICR = (1<<PCIE1) | (1<<PCIE2);
	PCMSK1 = (1<<PCINT8); 		//this corresponds to PORTB pin 0
	PCMSK2 = (1<<PCINT16);		//this correspondsto PORTC pin 0
	sei();
	
	init_uart();
		
	while(1){
		Delay(100);
		forward(200);
	}
}

