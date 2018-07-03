/*
 * metaldetector.h
 *
 * Created: 7/2/2018 2:03:31 PM
 *  Author: Shamus
 */ 


#ifndef METALDETECTOR_H_
#define METALDETECTOR_H_

#include <avr/io.h>

#define DEBUG_ADC	0

void init_adc(void);
float read_adc(uint8_t);

#endif /* METALDETECTOR_H_ */