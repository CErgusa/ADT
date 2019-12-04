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
#include "IntersectionManager.h"

// PA0 <--> PC
// PA1 <--> PC
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
  IntersectionManager_Init();
  
  //LCD_UART_Clear();
	while (1)
  {
    IntersectionManager_loop();
	} // End of the system loop
}
