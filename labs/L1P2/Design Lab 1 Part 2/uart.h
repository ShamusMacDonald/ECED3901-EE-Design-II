// UART.h
#ifndef UART_H
#define UART_H

#include <stdio.h>

#define F_CPU		14745600UL
#define BAUD		115200UL					// desired baud

int uart_putchar(char c,FILE *stream);
int uart_getchar(FILE *stream);
void init_uart(void);

#endif
