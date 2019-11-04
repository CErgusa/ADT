#include "Lidar.h"
#include <stdbool.h>
#include <stdint.h>



// PB0: UART1 Rx <- Lidar Tx
// PB1: UART1 Tx -> Lidar Rx
// PF2 -> M_SCTP(lidar): M1PWM6
// PF3 -> DEV_EN(lidar)
// PF4 -> M_EN(lidar)

#define timer_flag_health  0x00
#define timer_flag_info    0x10
volatile uint32_t flags = 0;

// PF2 -> M_SCTP(lidar): M1PWM6
// PF3 -> DEV_EN(lidar)
// PF4 -> M_EN(lidar)
void PortF_Init(void)
{
  // Activate clock for Port F
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;
  
  // Wait until Port F is ready
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};

  // Set input/output
  // set PF4-2 as output
  // set [4:3]
	GPIO_PORTF_DIR_R |= 0x18;
	
  // Alt function
  // Enable alt function: set [2]
  GPIO_PORTF_AFSEL_R |= 0x04;
  // Disable alt function: clear [4:3]
  GPIO_PORTF_AFSEL_R &= ~(0x18);
  
  // Configure the PMCn fields in the GPIOPCT
  GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;
  
  // Disable anlog function
  // clear [4:2]
  GPIO_PORTF_AMSEL_R &= ~0x1C;
    
  // Drive Strength: 8mA
  // set [4:3]
  GPIO_PORTF_DR8R_R |= 0x18;
  
  // Disable Open Drain
  // clear [4:3]
  GPIO_PORTF_ODR_R &= ~(0x18);  
    
  // Disable pull-up
  // clear [4:3]
  GPIO_PORTF_PUR_R &= ~(0x18);
    
  // Enable pull-down
  // set [4:3]
  GPIO_PORTF_PDR_R |= 0x18;
  
  // Enable digital function
  // set [4:2]
	GPIO_PORTF_DEN_R |= 0x1C;
}


void Lidar_Init(void)
{
  SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;
  PWM1_CTL_R = 0x00000000;//Disable PWM
  PWM1_0_GENA_R |= 0x000000C8;//Drive PWMa High,Action for Comparator is drive high
  PWM1_0_LOAD_R |= 0x00186A00;//Load 1600000 for 6 Hz frequency so that we can check blinking of LED.
  PWM1_0_CMPA_R |= 0x00000063;//25% Duty Cycle
  PWM1_0_CTL_R |= 0x00000001;//Start Timer for PWM Generator
  PWM1_ENABLE_R |= 0x00000001;//Start PWM  
}


void scan_lidar()
{
		// scan command [A5 60]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x60);
}
void restart_lidar()
{
		// restart command [A5 40]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x40);
}
void stop_lidar()
{
	// stop command [A5 65]
	UART1_OutChar(0xA5);
	UART1_OutChar(0x65);
}

// PC4 -> M_SCTP(lidar)
// PC5 -> M_EN(lidar)
void set_lidar_speed(void)
{
  // If negative speed, it means default
  //if (speed_percent < 0)
  //{
    // clear enable bit(PC5)
  //  GPIO_PORTC_DATA_R &= ~(0x20);
  //}
  // To control motor, set M_EN(PC5) high
  // Generate 10kHz sqaure wave..
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


void health()
{
	// health command [A5 91]
	UART1_OutChar(0xA5);
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
	UART1_OutChar(0xA5);
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
