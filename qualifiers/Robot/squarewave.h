/*
 * squarewave.h
 *
 * Created: 7/2/2018 1:48:07 PM
 *  Author: Shamus
 */ 


#ifndef SQUAREWAVE_H_
#define SQUAREWAVE_H_

#include <avr/io.h>

#define TIMER0 0
#define TIMER1 1
#define TIMER2 2

void squareWaveGen(char,char,float);

#endif /* SQUAREWAVE_H_ */