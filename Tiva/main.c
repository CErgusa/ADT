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
#include "timer.h"
#include "GDL.h"

#include <stdio.h>

int main(void)
{
	UART_Init();
	SysTick_Init();
	SSI_Init();
	ADC0_Init();
  ADC1_Init();
	UART1_Init();
	GDL_Init();
  Lidar_Init();
  
	UART_OutChar('>');
  //scan_lidar();
 
	while(1)
	{
  
    // lidar read
    // send
  
		//SSI_in();
      
    
	}
}
