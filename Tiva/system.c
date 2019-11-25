#include "system.h"

#include "SysTick (2).h"
#include "adc.h"
#include "SSI.h"
#include "UART1.h"
#include "Lidar.h"
#include "tm4c123gh6pm.h"
#include "GDL.h"

void system_init(void)
{
	SSI_Init();     // SPI communication
	UART_Init();    // TeraTerm UART communication
	SysTick_Init(); // System timer
	ADC0_Init();    // Batter cell voltage
  ADC1_Init();    // IR sensors
	UART1_Init();   // Lidar UART Communication
	GDL_Init();     // General Data Logic
}


// Jun change this as needed, this is our new main function
// I just wanted to move things towards an engine rather than
// always being in main!
int system_engine(void)
{
  // while (1) {SSI_in();}
	stop_lidar();
	SysTick_Wait1us(10);
	int response = scan_lidar();
	//int farts = UART1_InChar();
	
	if(response == RECEIVED)
	{
    UART1_enableInterrupts();
		while(1) { ; }
		// return NO_ERROR;
	}
	
	return ERROR;
}
