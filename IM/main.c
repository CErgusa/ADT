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
#include "adc.h"
#include "timer.h"
#include "tm4c123gh6pm.h"

// PA0 
// PA1 
// PB0(UART1 Rx) <--> LCD TXO
// PB1(UART1 Tx) <--> LCD RXI
// PD0(AIN7) -> Cell3 -> ADC0
// PD1(AIN6) -> Cell2 -> ADC0
// PD2(AIN5) -> Cell1 -> ADC0
// PD3(AIN4) -> Cell4 -> ADC1
// PE4(UART5 Rx) <--> Xbee DOUT
// PE5(UART5 Tx) <--> Xbee DIN

int main(void)
{
  // Initialize UART: boud rate, bits, stop bit, parity, flow control
	UART0_Init(); // Teraterm
  UART1_Init(); // LCD
  UART5_Init(); // Xbee
  ADC0_Init();
  ADC1_Init();

  //Xbee_SetDefault();  
  
  LCD_UART_Init();
  
  //UART0_OutString("SYSTEM START;");
  LCD_UART_OutString("SYSTEM START: IM;", true);
  
  //LCD_UART_Clear();
  // System loop
  //uint32_t cell = ADC_Get(1, CELL4_CHANNEL); // 127 ~ 170
  //UART5_InChar(); // Maker sure clear FIFO
	while (1)
  {
    char input = UART5_InChar();
    //cell = ADC_Get(1, CELL4_CHANNEL);
    if (input == 'T')
    {
      LCD_UART_OutChar(input);
      UART5_OutChar('M');
    }
	} // End of the system loop
}
