#include "Lidar.h"
#include <stdbool.h>
#include <stdint.h>
//#include "inc/hw_ints.h"
#include "inc/tm4c123gh6pm.h"
/*
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"*/

#define timer_flag_health  0x00
#define timer_flag_info    0x10
volatile uint32_t flags = 0;

void scan_lidar()
{
		// scan command [A5 60]
	UART1_OutChar('¥');
	UART1_OutChar('`');
}
void restart_lidar()
{
		// restart command [A5 40]
	UART1_OutChar('¥');
	UART1_OutChar('@');
}
void stop_lidar()
{
	// stop command [A5 65]
	UART1_OutChar('¥');
	UART1_OutChar('e');
}

void TIMER0A_Handler(int property)
{
	TIMER0_ICR_R |= (1<<0);
	//disable all interruptions
	
	//set my flag
	if(property == 0)
		flags = 0x01;
	if(property == 1)
		flags = 0x11;
	
}

// flag = 0: health flag
//      = 1: info flag
void timer(int ttime, int flag)
{
	SYSCTL_RCGCTIMER_R |= 1;            /* enable clock (all modules) in Run mode */
  TIMER0_CTL_R = 0;                   /* disable Timer before init. */
  TIMER0_CFG_R = 0x04;                /* 16-bit option, hassard said we don't need 32 */
  TIMER0_TAMR_R = 0b100001;           /* one-shot mode, default count down; enabling TAMIE interruption */
  TIMER0_TAILR_R = 16000 * ttime - 1; /* Timer A interval load value register */
  TIMER0_ICR_R = 0x1;                 /* clear the TimerA timeout flag */
  TIMER0_CTL_R |= 0x01;               /* enable Timer A after initialization */
	TIMER0_IMR_R |= (1<<0);             /* set time-out interrupt in the GPTM Interrupt Mask Register (GPTMIMR) */
	
  while(1)
	{
		if((TIMER0_RIS_R & 0x1) == 1)     /* wait for TimerA timeout flag to set */
		{
			UART_OutChar('0');
			//TIMER0A_Handler(flag);
			break;
		}
	}
}
/*
volatile uint32_t g_ui32Counter = 0;
void timer1(void)
{
	uint32_t ui32PrevCount = 0;
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);


	// Configure Timer0B as a 16-bit periodic timer.
	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);


	// Set the Timer0B load value to 1ms.
	TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet() / 1000);


	// Enable the Timer0B interrupt on the processor (NVIC).
	IntEnable(INT_TIMER0B);


	// Enable Timer0B.
	TimerEnable(TIMER0_BASE, TIMER_B);


	// Enable processor interrupts.
	IntMasterEnable();


	// Configure the Timer0B interrupt for timer timeout.
	TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

	// Initialize the interrupt counter.
	g_ui32Counter = 0;
	
	// Loop forever while the Timer0B runs.
	while(1)
	{
			// If the interrupt count changed, print the new value
		 if(ui32PrevCount != g_ui32Counter)
		{
				ui32PrevCount = g_ui32Counter;
		}
	}
}

void Timer0BIntHandler(void)
{
	// Clear the timer interrupt flag.
	 TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
	// Update the periodic interrupt counter.
	 g_ui32Counter++;
}*/

void health()
{
	// health command [A5 91]
	UART1_OutChar('¥');
	UART1_OutChar(0x91);
	
	int in = 0;
	UART_OutChar('A');
	timer(100, 0);
	UART_OutChar('B');
	while(flags == 0x00)
	{
		UART_OutChar('C');
		in = UART1_InChar(); // take in input
		// convert from # to ascii
		UART_OutChar(in); // output to terterm
	}
	
	flags = 0;
}

void device_status()
{
		// device info [A5 90]
	UART1_OutChar('¥');
	UART1_OutChar(0x90);
	
	int in = 0;
	timer(10,1);
	while(flags == 0x00)
	{
		in = UART1_InChar(); // take in input
		// convert from # to ascii
		UART_OutChar(in); // output to terterm
	}
	
	flags = 0;
}

