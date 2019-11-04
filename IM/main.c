/******************************************************************************/
/*
	Project Name: Instrumentation of Self-driving Vehicles
	author: Junsung Ahn
  email: junsung.ahn@digipen.edu
  DigiPen login: junsung.ahn
  Course: ECE310L
*/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "UART.h"
#include "SPI.h"
#include "LCD_SPI.h"
#include "LCD_UART.h"
#include "timer.h"
#include "tm4c123gh6pm.h"


#define MODE_SEARCH 0x10
#define MODE_EDIT 0x20


int main(void)
{
  // Arrays of characters which have maximum possible length
  char ModeSelection;
  
  // Initialize UART: boud rate, bits, stop bit, parity, flow control
	UART0_Init();
  UART1_Init();
  SSI1_Init();	//Initialize SPI
  SSI0_Init();

  LCD_UART_Init();
  
  LCD_UART_OutString("SYSTEM START;", true);
  LCD_UART_OutString(" -mSD:Addressing..;", true);

  LCD_UART_Clear();
  
  // System loop
	while (1)
  {
    LCD_UART_OutString("<SELECT THE MODE>;", true);
    LCD_UART_OutString("TOP:    SEARCH;", true);
    LCD_UART_OutString("BOTTOM: EDIT;", true);
    while (ModeSelection == ';')
    {

    }
	} // End of the system loop
}
