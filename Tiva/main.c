#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "SysTick (2).h"
#include "adc.h"
#include "SSI.h"
#include "UART1.h"
#include "Lidar.h"
#include "tm4c123gh6pm.h"
//#include "timer.h"
#include "GDL.h"

#include <stdio.h>

int main(void)
{
	SSI_Init();
	UART_Init();
	SysTick_Init();
	ADC0_Init();
  ADC1_Init();
	UART1_Init();
	GDL_Init();
  //Lidar_Init();
  restart_lidar();
  //stop_lidar();
  
  //SysTick_Wait1us(10);
  
  int response = scan_lidar();
  
  //int lidar[0x94];
  
  //if (response == 0xA55A)
  //{
  //  while(1) {}
  //}
  
	UART_OutChar('>');
  
	while(1)
	{
    // lidar read
    // send
  
		//SSI_in();
      
    
	}
}
