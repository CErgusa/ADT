#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RCGCADC  (*((volatile unsigned long *)0x400FE638))
#define RCGCGPIO (*((volatile unsigned long *)0x400FE608))
	
/*****************************PORT A***********************************/
#define GPIO_PORTA_DATA        (*((volatile unsigned long *)0x40004000))
#define GPIO_PORTA_DIR         (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_DEN         (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AFSEL       (*((volatile unsigned long *)0x40004420))
/**********************************************************************/
/*****************************PORT D***********************************/
#define GPIO_PORTD_DATA  			 (*((volatile unsigned long *)0x40007000))
#define GPIO_PORTD_DIR         (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL       (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN         (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_AMSEL       (*((volatile unsigned long *)0x40007528))
/**********************************************************************/
#define CELL1_CHANNEL 0x07
#define CELL2_CHANNEL 0x06
#define CELL3_CHANNEL 0x05
#define IR1_CHANNEL   0x04
#define IR2_CHANNEL   0x02
#define IR3_CHANNEL   0x01

void ADC0_Init(void);
void ADC1_Init(void);

uint32_t ADC0_Distance1(void);

uint32_t ADC0_Distance2(void);

uint32_t ADC0_Distance3(void);

uint32_t ADC_Get(int ADC_INDEX, int CHANNEL);
