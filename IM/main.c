/******************************************************************************/
/*
	Project Name: Intersection Manager
  Main project: Instrumentation of Self-driving Vehicles
	author: Junsung Ahn
  email: junsung.ahn@digipen.edu
  DigiPen login: junsung.ahn
  Course: ECE310L
*/
/******************************************************************************/
#include "UART.h"
#include "SPI.h"
#include "Xbee.h"
#include "LCD_SPI.h"
#include "LCD_UART.h"
#include "timer.h"
#include "tm4c123gh6pm.h"


int main(void)
{
  // Initialize UART: boud rate, bits, stop bit, parity, flow control
	UART0_Init(); // Teraterm
  UART1_Init(); // LCD
  UART5_Init(); // Xbee

  Xbee_SetDefault();  
  
  LCD_UART_Init();
  
  //UART5_OutString("SYSTEM START;");
  LCD_UART_OutString("SYSTEM START;", true);
  LCD_UART_OutString("IManager;", false);
  
  //LCD_UART_Clear();
  // System loop
	while (1)
  {
    char input = UART5_InChar();
    if (input != 0x00)
    {
      LCD_UART_OutChar(input);
    }
	} // End of the system loop
}
