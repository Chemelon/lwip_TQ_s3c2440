#ifndef __USART_H
#define __USART_H
#include <stdio.h>


void uart0_init(void);
void usart_putc(unsigned char c);
// unsigned char getc(void);
// void puts(char * str);
void puthex(unsigned int val);
int isDigit(unsigned char c);
int isLetter(unsigned char c);

#endif
