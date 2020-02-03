#include "GDL.h"
#include "tm4c123gh6pm.h"

// Using Ports
// Port A: UART0(PA1-0), SSI0(PA5-2)
// Port B: UART1(PB1-0: lidar)
// Port C: General Digital Logic(PC7-4)
// Port D: ADC(PD3-0), General Digital Logic(PD6)
// Port E: ADC(PE2-1)
// Port F: Lidar motor control(PF4-2)

// GDL(General Digital Logic) 

// PC4 -> GDL0
// PC5 -> GLD1
// PC6 -> GDL2
// PC7 -> GDL3
// PD6 -> GDL4



// PC4 -> GDL0
// PC5 -> GLD1
// PC6 -> GDL2
// PC7 -> GDL3
void PortC_Init(void)
{
  // Activate clock for Port C
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOC; 
  
  // Wait until Port C is ready
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R2) == 0){};

  // Set input/output
  // For now, set PC7-4 as output
  // set [7:4]
	GPIO_PORTC_DIR_R |= 0xF0;
	
  // Disable alt function, so just GPIO
  // clear [7:4]
  GPIO_PORTC_AFSEL_R &= ~(0xF0);
    
  // Drive Strength: 8mA
  // set [7:4]
  GPIO_PORTC_DR8R_R |= 0xF0;
    
  // Disable Open Drain
  // clear [7:4]
  GPIO_PORTC_ODR_R &= ~(0xF0);  
    
  // Disable pull-up
  // clear [7:4]
  GPIO_PORTC_PUR_R &= ~(0xF0);
    
  // Enable pull-down
  // set [7:4]
  GPIO_PORTC_PDR_R |= 0xF0;
  
  // Enable digital function
  // set [7:4]
	GPIO_PORTC_DEN_R |= 0xF0;

	// Clear
  GPIO_PORTC_DATA_R &= ~(0xF0);
}


void GDL_Init(void)
{
  PortC_Init();
}

// GDL(General Digital Logic) 
// PC4 -> GDL0 -> BCM26
// PC5 -> GDL1 -> BCM19
// PC6 -> GDL2 -> BCM5
// PC7 -> GDL3 -> BCM0
// PD6 -> GDL4 -> BCM22
void GDL_Send(unsigned char data_max5bits)
{
  // Parse given data
  unsigned char GDL[5];
  int i;
  for (i = 0; i < 5; ++i)
  {
    GDL[i] = ((data_max5bits & (0x01 << i)) >> i);
  }
  
  // Send parsed data to assiciating pin
  // PC4 -> GDL0
  // PC5 -> GLD1
  // PC6 -> GDL2
  // PC7 -> GDL3
  unsigned char PCs = ( (GDL[3] << 3) | (GDL[2] << 2) |(GDL[1] << 1) | GDL[0] ) << 4;
  
  // PD6 -> GDL4
  unsigned char PDs = GDL[4] << 6;
  
  // Clear assiciating bit field before sending
  GPIO_PORTC_DATA_R &= ~(0xF0);
  GPIO_PORTD_DATA_R &= ~(0x40);

  GPIO_PORTC_DATA_R |= PCs;
  GPIO_PORTD_DATA_R |= PDs;
}

