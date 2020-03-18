#include "GDL.h"
#include <stdint.h> // uint_32
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
  // PC7-5 outputs, PC4 input
  // For now, set PC7-4 as output
  // set [7:4]
	GPIO_PORTC_DIR_R |= 0xF0;
  // clear [4] : for PC4 as input
	GPIO_PORTC_DIR_R &= ~(0x10);
  
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


void GDL_init(void)
{
  PortC_Init();
}

// GDL(General Digital Logic) 
// PC4 -> GDL0 -> BCM26
// PC5 -> GDL1 -> BCM19
// PC6 -> GDL2 -> BCM5
// PC7 -> GDL3 -> BCM0
// PD6 -> GDL4 -> BCM22
void GDL_send(unsigned char data_max5bits)
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

unsigned char GDL_read(unsigned char index)
{
  // PC4 -> GDL[0]
  // PC5 -> GDL[1]
  // PC6 -> GDL[2]
  // PC7 -> GDL[3]
  // PD6 -> GDL[4]
  unsigned char value_pos = 0;
  switch (index)
  {
    case 0: // GDL[0] - PC4
    {
      value_pos = (GPIO_PORTC_DATA_R & (0x01 << 4));
      value_pos = (value_pos >> 4);
      break;
    }
    case 1: // GDL[1] - PC5 
    {
      value_pos = (GPIO_PORTC_DATA_R & (0x01 << 5));
      value_pos = (value_pos >> 5);
      break;
    }
    case 2: // GDL[2] - PC6
    {
      value_pos = (GPIO_PORTC_DATA_R & (0x01 << 6));
      value_pos = (value_pos >> 6);
      break;
    }
    case 3: // GDL[3] - PC7
    {
      value_pos = (GPIO_PORTC_DATA_R & (0x01 << 7));
      value_pos = (value_pos >> 7);
      break;
    }
    case 4: // GDL[4] - PD6
    {
      value_pos = (GPIO_PORTD_DATA_R & (0x01 << 6));
      value_pos = (value_pos >> 6);
      break;
    }
    default: break;
  }
  return value_pos;
}

