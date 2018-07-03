/*
 * metaldetector.c
 *
 * Created: 7/2/2018 2:03:44 PM
 *  Author: Shamus
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "metaldetector.h"
#include "uart.h"

#define ADC_VALUE(x)	( (x)/1024. * 5)	

void init_adc(void)
{
	ADMUX = (1<<REFS0);
//	Enable ADC0, Prescale 128 
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

float read_adc(uint8_t channel)
{
	float adc_val;
// Keep channel between 0-7
	channel &= 0x07;
	ADMUX = (ADMUX & 0xF8) | channel;
	ADCSRA |= (1<<ADSC);
	
	while(ADCSRA & (1<<ADSC));
	
	adc_val = ADC_VALUE(ADC);
	
	#ifdef DEBUG_ADC
	printf_P(PSTR("%d\t= %7.3f\n"), ADC, adc_val);
	#endif
	
	return adc_val;
}