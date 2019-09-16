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
#include <stdio.h>


int main(void)
{
	UART_Init();
	SysTick_Init();
	SSI_Init();
	ADC0_Init();
	UART1_Init();
	
	UART_OutChar('>');
	
	while(1)
	{
		SSI_in();
	}
}

